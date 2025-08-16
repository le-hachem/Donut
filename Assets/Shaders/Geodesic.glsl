#version 430
layout(local_size_x = 16, local_size_y = 16) in;

layout(binding = 0, rgba8) writeonly uniform image2D outImage;
layout(std140, binding = 1) uniform Camera 
{
    vec3  camPos;     float _pad0;
    vec3  camRight;   float _pad1;
    vec3  camUp;      float _pad2;
    vec3  camForward; float _pad3;
    float tanHalfFov;
    float aspect;
    bool  moving;
    int   _pad4;
} cam;

layout(std140, binding = 2) uniform Disk 
{
    float disk_r1;
    float disk_r2;
    float disk_num;
    float thickness;
};

layout(std140, binding = 3) uniform Objects 
{
    int    numObjects;
    vec4   objPosRadius[16];
    vec4   objColor[16];
    float  mass[16]; 
};

layout(std140, binding = 4) uniform Simulation 
{
    int   maxStepsMoving;
    int   maxStepsStatic;
    float earlyExitDistance;
    int   _pad0;
};

const float  SagA_rs  = 1.269e10;
const float  D_LAMBDA = 1e7;
const double ESCAPE_R = 1e30;

const int   DEFAULT_MAX_STEPS_MOVING   = 12000;
const int   DEFAULT_MAX_STEPS_STATIC   = 8000;
const float DEFAULT_EARLY_EXIT_DISTANCE = 2e12;

const float MIN_STEP_SIZE = 1e6;
const float MAX_STEP_SIZE = 5e7;
const float STEP_ADAPTATION_FACTOR = 1.5;

vec4 objectColor = vec4(0.0);
vec3 hitCenter = vec3(0.0);
float hitRadius = 0.0;

struct Ray 
{
    float x, y, z;
    float r, theta, phi;
    float dr, dtheta, dphi;
    float E, L;
};

Ray InitRay(vec3 pos, vec3 dir) 
{
    Ray ray;
    ray.x     = pos.x; 
    ray.y     = pos.y; 
    ray.z     = pos.z;
    ray.r     = length(pos);
    ray.theta = acos(pos.z / ray.r);
    ray.phi   = atan(pos.y, pos.x);

    float dx = dir.x;
    float dy = dir.y;
    float dz = dir.z;

    ray.dr     = sin(ray.theta)*cos(ray.phi)*dx + 
                 sin(ray.theta)*sin(ray.phi)*dy + 
                 cos(ray.theta)*dz;

    ray.dtheta = (cos(ray.theta)*cos(ray.phi)*dx + 
                  cos(ray.theta)*sin(ray.phi)*dy - 
                  sin(ray.theta)*dz) / ray.r;

    ray.dphi   = (-sin(ray.phi)*dx + cos(ray.phi)*dy) / 
                 (ray.r * sin(ray.theta));

    ray.L = ray.r * ray.r * sin(ray.theta) * ray.dphi;
    float f = 1.0 - SagA_rs / ray.r;
    float dt_dL = sqrt((ray.dr*ray.dr)/f + 
                       ray.r*ray.r*(ray.dtheta*ray.dtheta + 
                       sin(ray.theta)*sin(ray.theta)*
                       ray.dphi*ray.dphi));
    ray.E = f * dt_dL;

    return ray;
}

bool Intercept(Ray ray, float rs) 
{
    return ray.r <= rs;
}

bool InterceptObject(Ray ray) 
{
    vec3 P = vec3(ray.x, ray.y, ray.z);
    
    for (int i = 0; i < numObjects; ++i) 
    {
        vec3  center = objPosRadius[i].xyz;
        float radius = objPosRadius[i].w;
        
        float distSq = dot(P - center, P - center);
        if (distSq > radius * radius * 4.0) 
            continue;

        if (distSq <= radius * radius)
        {
            objectColor = objColor[i];
            hitCenter = center;
            hitRadius = radius;
            return true;
        }
    }

    return false;
}

void GeodesicRHS(Ray ray, out vec3 d1, out vec3 d2) 
{
    float r      = ray.r;
    float theta  = ray.theta;
    float dr     = ray.dr;
    float dtheta = ray.dtheta;
    float dphi   = ray.dphi;
    float f      = 1.0 - SagA_rs / r;
    float dt_dL  = ray.E / f;

    d1 = vec3(dr, dtheta, dphi);
    d2.x = - (SagA_rs / (2.0 * r*r)) * f * dt_dL * dt_dL
           + (SagA_rs / (2.0 * r*r * f)) * dr * dr
           + r * (dtheta*dtheta + sin(theta)*sin(theta)*dphi*dphi);
    d2.y = -2.0*dr*dtheta/r + sin(theta)*cos(theta)*dphi*dphi;
    d2.z = -2.0*dr*dphi/r - 2.0*cos(theta)/(sin(theta)) * dtheta * dphi;
}

void RK4Step(inout Ray ray, float dL) 
{
    vec3 k1a, k1b;
    GeodesicRHS(ray, k1a, k1b);

    ray.r      += dL * k1a.x;
    ray.theta  += dL * k1a.y;
    ray.phi    += dL * k1a.z;
    ray.dr     += dL * k1b.x;
    ray.dtheta += dL * k1b.y;
    ray.dphi   += dL * k1b.z;

    ray.x = ray.r * sin(ray.theta) * cos(ray.phi);
    ray.y = ray.r * sin(ray.theta) * sin(ray.phi);
    ray.z = ray.r * cos(ray.theta);
}

bool CrossesEquatorialPlane(vec3 oldPos, vec3 newPos) 
{
    bool crossed = (oldPos.y * newPos.y < 0.0);
    float r = length(vec2(newPos.x, newPos.z));
    return crossed && (r >= disk_r1 && r <= disk_r2);
}

float CalculateAdaptiveStepSize(Ray ray, float baseStepSize) 
{
    float r_factor         = clamp(ray.r / (SagA_rs * 10.0), 0.1, 1.0);
    float curvature        = length(vec3(ray.dr, ray.dtheta * ray.r, ray.dphi * ray.r * sin(ray.theta)));
    float curvature_factor = clamp(1e12 / (curvature + 1e6), 0.1, 2.0);
    
    return clamp(baseStepSize * r_factor * curvature_factor, MIN_STEP_SIZE, MAX_STEP_SIZE);
}

void main() 
{
    ivec2 pix  = ivec2(gl_GlobalInvocationID.xy);
    int WIDTH  = imageSize(outImage).x;
    int HEIGHT = imageSize(outImage).y;

    if (pix.x >= WIDTH || 
        pix.y >= HEIGHT) 
        return;

    float u = (2.0 * (pix.x + 0.5) / WIDTH - 1.0) * 
              cam.aspect * cam.tanHalfFov;
    float v = (1.0 - 2.0 * (pix.y + 0.5) / HEIGHT) * 
               cam.tanHalfFov;
    vec3 dir = normalize(u * cam.camRight - 
                         v * cam.camUp    + 
                         cam.camForward);
    Ray ray = InitRay(cam.camPos, dir);

    vec4  color   = vec4(0.0);
    vec3  prevPos = vec3(ray.x, ray.y, ray.z);
    float lambda  = 0.0;

    bool hitBlackHole = false;
    bool hitDisk      = false;
    bool hitObject    = false;

    int maxSteps = cam.moving ? maxStepsMoving : maxStepsStatic;
    
    if (maxSteps <= 0)
        maxSteps = cam.moving ? DEFAULT_MAX_STEPS_MOVING : DEFAULT_MAX_STEPS_STATIC;
    
    float cameraDistance = length(cam.camPos);
    if (cameraDistance > 2e12)
        maxSteps = maxSteps / 2;
    else if (cameraDistance > 1e12)
        maxSteps = int(maxSteps * 0.75);

    float initialEscapeVelocity = sqrt(2.0 * SagA_rs / ray.r);
    if (ray.dr > initialEscapeVelocity * 0.95 && 
        ray.r  > SagA_rs * 200.0)
        maxSteps = maxSteps / 2;

    float currentStepSize = D_LAMBDA;
    int objectCheckInterval = 5;

    for (int i = 0; i < maxSteps; ++i) 
    {
        float exitDistance = earlyExitDistance > 0.0 ? earlyExitDistance : DEFAULT_EARLY_EXIT_DISTANCE;
        if (ray.r > exitDistance) 
            break;
        if (ray.r > ESCAPE_R) 
            break;
        
        if (Intercept(ray, SagA_rs)) 
        { 
            hitBlackHole = true; 
            break; 
        }
        
        currentStepSize = CalculateAdaptiveStepSize(ray, D_LAMBDA);
        
        RK4Step(ray, currentStepSize);
        lambda += currentStepSize;

        vec3 newPos = vec3(ray.x, ray.y, ray.z);
        
        if (CrossesEquatorialPlane(prevPos, newPos)) 
        { 
            hitDisk = true; 
            break; 
        }
        
        if (i % objectCheckInterval == 0 && InterceptObject(ray)) 
        { 
            hitObject = true; 
            break; 
        }
        
        prevPos = newPos;
        
        if (ray.dr > 0.0 && ray.r > SagA_rs * 100.0 && lambda > 2e8)
            break;
    }

    if (hitDisk) 
    {
        double r = length(vec3(ray.x, ray.y, ray.z)) / disk_r2;
        vec3 diskColor = vec3(1.0, r, 0.2);
        color = vec4(diskColor, r);

    } else if (hitBlackHole)
        color = vec4(0.0, 0.0, 0.0, 1.0);
    else if (hitObject) 
    {
        vec3 P = vec3(ray.x, ray.y, ray.z);
        vec3 N = normalize(P - hitCenter);
        vec3 V = normalize(cam.camPos - P);

        float ambient   = 0.1;
        float diff      = max(dot(N, V), 0.0);
        float intensity = ambient + (1.0 - ambient) * diff;
        vec3  shaded    = objectColor.rgb * intensity;

        color = vec4(shaded, objectColor.a);
    } else
        color = vec4(0.0);

    imageStore(outImage, pix, color);
}