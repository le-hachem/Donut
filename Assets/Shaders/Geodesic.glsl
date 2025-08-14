#version 430
layout(local_size_x = 16, local_size_y = 16) in;

layout(binding = 0, rgba8) writeonly uniform image2D outImage;

layout(std140, binding = 1) uniform Camera 
{
    vec3 camPos;     float _pad0;
    vec3 camRight;   float _pad1;
    vec3 camUp;      float _pad2;
    vec3 camForward; float _pad3;
    float tanHalfFov;
    float aspect;
    bool moving;
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
    int numObjects;
    vec4 objPosRadius[16];
    vec4 objColor[16];
    float mass[16]; 
};

const float SagA_rs = 1.269e10;
const float D_LAMBDA = 1e7;
const double ESCAPE_R = 1e30;

vec4 objectColor = vec4(0.0);
vec3 hitCenter = vec3(0.0);
float hitRadius = 0.0;

struct Ray 
{
    float x, y, z, radius, theta, phi;
    float dRadius, dTheta, dPhi;
    float energy, angularMomentum;
};

Ray InitRay(vec3 position, vec3 direction) 
{
    Ray ray;
    ray.x = position.x; 
    ray.y = position.y; 
    ray.z = position.z;
    ray.radius = length(position);
    ray.theta = acos(position.z / ray.radius);
    ray.phi = atan(position.y, position.x);

    float dx = direction.x, dy = direction.y, dz = direction.z;
    ray.dRadius = sin(ray.theta) * cos(ray.phi) * dx + sin(ray.theta) * sin(ray.phi) * dy + cos(ray.theta) * dz;
    ray.dTheta = (cos(ray.theta) * cos(ray.phi) * dx + cos(ray.theta) * sin(ray.phi) * dy - sin(ray.theta) * dz) / ray.radius;
    ray.dPhi = (-sin(ray.phi) * dx + cos(ray.phi) * dy) / (ray.radius * sin(ray.theta));

    ray.angularMomentum = ray.radius * ray.radius * sin(ray.theta) * ray.dPhi;
    float f = 1.0 - SagA_rs / ray.radius;
    float dt_dL = sqrt((ray.dRadius * ray.dRadius) / f + ray.radius * ray.radius * (ray.dTheta * ray.dTheta + sin(ray.theta) * sin(ray.theta) * ray.dPhi * ray.dPhi));
    ray.energy = f * dt_dL;

    return ray;
}

bool Intercept(Ray ray, float schwarzschildRadius) 
{
    return ray.radius <= schwarzschildRadius;
}

bool InterceptObject(Ray ray) 
{
    vec3 position = vec3(ray.x, ray.y, ray.z);
    
    for (int i = 0; i < numObjects; ++i) 
    {
        vec3 center = objPosRadius[i].xyz;
        float radius = objPosRadius[i].w;
        
        if (distance(position, center) <= radius) 
        {
            objectColor = objColor[i];
            hitCenter = center;
            hitRadius = radius;
            return true;
        }
    }
    
    return false;
}

void GeodesicRHS(Ray ray, out vec3 derivatives1, out vec3 derivatives2) 
{
    float radius = ray.radius, theta = ray.theta;
    float dRadius = ray.dRadius, dTheta = ray.dTheta, dPhi = ray.dPhi;
    float f = 1.0 - SagA_rs / radius;
    float dt_dL = ray.energy / f;

    derivatives1 = vec3(dRadius, dTheta, dPhi);
    derivatives2.x = -(SagA_rs / (2.0 * radius * radius)) * f * dt_dL * dt_dL
         + (SagA_rs / (2.0 * radius * radius * f)) * dRadius * dRadius
         + radius * (dTheta * dTheta + sin(theta) * sin(theta) * dPhi * dPhi);
    derivatives2.y = -2.0 * dRadius * dTheta / radius + sin(theta) * cos(theta) * dPhi * dPhi;
    derivatives2.z = -2.0 * dRadius * dPhi / radius - 2.0 * cos(theta) / sin(theta) * dTheta * dPhi;
}

void RK4Step(inout Ray ray, float deltaLambda) 
{
    vec3 k1a, k1b;
    GeodesicRHS(ray, k1a, k1b);

    ray.radius += deltaLambda * k1a.x;
    ray.theta += deltaLambda * k1a.y;
    ray.phi += deltaLambda * k1a.z;
    ray.dRadius += deltaLambda * k1b.x;
    ray.dTheta += deltaLambda * k1b.y;
    ray.dPhi += deltaLambda * k1b.z;

    ray.x = ray.radius * sin(ray.theta) * cos(ray.phi);
    ray.y = ray.radius * sin(ray.theta) * sin(ray.phi);
    ray.z = ray.radius * cos(ray.theta);
}

bool CrossesEquatorialPlane(vec3 oldPosition, vec3 newPosition) 
{
    bool crossed = (oldPosition.y * newPosition.y < 0.0);
    float radius = length(vec2(newPosition.x, newPosition.z));
    return crossed && (radius >= disk_r1 && radius <= disk_r2);
}

void main() 
{
    ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
    int width = imageSize(outImage).x;
    int height = imageSize(outImage).y;
    
    if (pixel.x >= width || pixel.y >= height) 
        return;

    float u = (2.0 * (pixel.x + 0.5) / width - 1.0) * cam.aspect * cam.tanHalfFov;
    float v = (1.0 - 2.0 * (pixel.y + 0.5) / height) * cam.tanHalfFov;
    vec3 direction = normalize(u * cam.camRight - v * cam.camUp + cam.camForward);
    Ray ray = InitRay(cam.camPos, direction);

    vec4 color = vec4(0.0);
    vec3 prevPosition = vec3(ray.x, ray.y, ray.z);
    float lambda = 0.0;

    bool hitBlackHole = false;
    bool hitDisk = false;
    bool hitObject = false;

    int steps = cam.moving ? 60000 : 60000;

    for (int i = 0; i < steps; ++i) 
    {
        if (Intercept(ray, SagA_rs)) 
        { 
            hitBlackHole = true; 
            break; 
        }
        
        RK4Step(ray, D_LAMBDA);
        lambda += D_LAMBDA;

        vec3 newPosition = vec3(ray.x, ray.y, ray.z);
        
        if (CrossesEquatorialPlane(prevPosition, newPosition)) 
        { 
            hitDisk = true; 
            break; 
        }
        
        if (InterceptObject(ray)) 
        { 
            hitObject = true; 
            break; 
        }
        
        prevPosition = newPosition;
        
        if (ray.radius > ESCAPE_R) 
            break;
    }

    if (hitDisk) 
    {
        double radius = length(vec3(ray.x, ray.y, ray.z)) / disk_r2;
        vec3 diskColor = vec3(1.0, radius, 0.2);
        color = vec4(diskColor, radius);
    } 
    else if (hitBlackHole) 
        color = vec4(0.0, 0.0, 0.0, 1.0);
    else if (hitObject) 
    {
        vec3 position = vec3(ray.x, ray.y, ray.z);
        vec3 normal = normalize(position - hitCenter);
        vec3 viewDirection = normalize(cam.camPos - position);
        float ambient = 0.1;
        float diffuse = max(dot(normal, viewDirection), 0.0);
        float intensity = ambient + (1.0 - ambient) * diffuse;
        vec3 shaded = objectColor.rgb * intensity;
        color = vec4(shaded, objectColor.a);
    } 
    else 
        color = vec4(0.0);

    imageStore(outImage, pixel, color);
}