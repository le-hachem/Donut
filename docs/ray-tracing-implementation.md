# Ray Tracing in Curved Spacetime: A Detailed Overview

This ray tracer simulates how light behaves near a black hole, including the effects of curved spacetime and interactions with objects like stars and an accretion disk. The implementation follows a step-by-step approach, which includes initializing rays, tracing their paths through spacetime, detecting intersections with objects, and finally rendering the scene.

## 1. Ray Initialization

The first step in ray tracing is to create rays originating from the camera that will eventually traverse through spacetime. Each ray represents a possible path of light.

### Camera Setup

The camera is positioned in three-dimensional space and is defined by its orientation and field of view. It has the following parameters:

```glsl
layout(std140, binding = 1) uniform Camera 
{
    vec3  camPos;       float _pad0;
    vec3  camRight;     float _pad1;
    vec3  camUp;        float _pad2;
    vec3  camForward;   float _pad3;
    float tanHalfFov;   // Field of view
    float aspect;       // Aspect ratio
    bool  moving;       // Whether the camera is moving
    int   _pad4;
} cam;
```

- `camPos`: The 3D position of the camera.
- `camRight`, `camUp`, `camForward`: Orthonormal vectors defining the camera's orientation.
- `tanHalfFov` and `aspect`: Determine the camera’s field of view and the shape of the image plane.

### Ray Generation per Pixel

For each pixel on the image, we calculate the corresponding direction in world space and generate a ray pointing in that direction:

```glsl
float u = (2.0 * (pix.x + 0.5) / WIDTH - 1.0) * cam.aspect * cam.tanHalfFov;
float v = (1.0 - 2.0 * (pix.y + 0.5) / HEIGHT) * cam.tanHalfFov;
vec3 dir = normalize(u * cam.camRight - v * cam.camUp + cam.camForward);
Ray ray = InitRay(cam.camPos, dir);
```

Here, `u` and `v` are normalized coordinates on the image plane. The `InitRay` function takes the camera position and the computed direction to create a ray in both Cartesian and spherical coordinates.

### Ray Structure

Each ray contains not only the standard 3D Cartesian position but also spherical coordinates and velocity components, which are necessary for simulating curved spacetime:

```glsl
struct Ray 
{
    float x, y, z;          // Cartesian coordinates
    float r, theta, phi;    // Spherical coordinates
    float dr, dtheta, dphi; // Radial and angular velocities
    float E, L;             // Conserved quantities in Schwarzschild geometry
};
```

### Converting Cartesian to Spherical Coordinates

The `InitRay` function converts the position and direction of the ray into spherical coordinates and computes initial velocities:

```glsl
Ray InitRay(vec3 pos, vec3 dir) 
{
    Ray ray;
    ray.x     = pos.x; 
    ray.y     = pos.y; 
    ray.z     = pos.z;

    // Spherical coordinates
    ray.r     = length(pos);
    ray.theta = acos(pos.z / ray.r);
    ray.phi   = atan(pos.y, pos.x);

    // Convert direction vector to spherical velocities
    float dx = dir.x;
    float dy = dir.y;
    float dz = dir.z;

    ray.dr     = sin(ray.theta)*cos(ray.phi)*dx + sin(ray.theta)*sin(ray.phi)*dy + cos(ray.theta)*dz;
    ray.dtheta = (cos(ray.theta)*cos(ray.phi)*dx + cos(ray.theta)*sin(ray.phi)*dy - sin(ray.theta)*dz) / ray.r;
    ray.dphi   = (-sin(ray.phi)*dx + cos(ray.phi)*dy) / (ray.r * sin(ray.theta));

    // Calculate conserved quantities (energy E and angular momentum L)
    ray.L = ray.r * ray.r * sin(ray.theta) * ray.dphi;
    float f = 1.0 - SagA_rs / ray.r;
    float dt_dL = sqrt((ray.dr*ray.dr)/f + ray.r*ray.r*(ray.dtheta*ray.dtheta + sin(ray.theta)*sin(ray.theta)*ray.dphi*ray.dphi));
    ray.E = f * dt_dL;

    return ray;
}
```

## 2. Geodesic Integration

Once the ray is initialized, we trace its path through curved spacetime using the Schwarzschild metric. This requires solving the geodesic equations for the ray.

### Integration Loop

The ray is advanced step by step using a numerical integrator (Runge-Kutta 4th order). The loop continues until the ray either escapes the scene, falls into the black hole, or intersects an object.

```glsl
for (int i = 0; i < maxSteps; ++i) 
{
    if (ray.r > exitDistance) break;
    if (ray.r > ESCAPE_R) break;
    if (Intercept(ray, SagA_rs)) { hitBlackHole = true; break; }
    
    currentStepSize = CalculateAdaptiveStepSize(ray, D_LAMBDA);
    RK4Step(ray, currentStepSize);
    lambda += currentStepSize;

    vec3 newPos = vec3(ray.x, ray.y, ray.z);
    if (CrossesEquatorialPlane(prevPos, newPos)) { hitDisk = true; break; }
    if (i % objectCheckInterval == 0 && InterceptObject(ray)) { hitObject = true; break; }

    prevPos = newPos;
}
```

### Runge-Kutta 4 (RK4)

The RK4 integrator provides high accuracy by computing intermediate slopes and averaging them to advance the ray:

```glsl
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

    // Update Cartesian coordinates
    ray.x = ray.r * sin(ray.theta) * cos(ray.phi);
    ray.y = ray.r * sin(ray.theta) * sin(ray.phi);
    ray.z = ray.r * cos(ray.theta);
}
```

### Geodesic Derivatives

The function `GeodesicRHS` computes the derivatives needed for integration:

```glsl
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
```

## 3. Intersection Testing

Rays can intersect three types of entities: the black hole, spherical objects, and the accretion disk.

### Black Hole Intersection

A ray hitting the event horizon is considered absorbed:

```glsl
bool Intercept(Ray ray, float rs) 
{
    return ray.r <= rs;
}
```

### Object Intersection

The scene can contain multiple spherical objects such as stars or planets:

```glsl
bool InterceptObject(Ray ray) 
{
    vec3 P = vec3(ray.x, ray.y, ray.z);
    
    for (int i = 0; i < numObjects; ++i) 
    {
        vec3 center = objPosRadius[i].xyz;
        float radius = objPosRadius[i].w;
        
        float distSq = dot(P - center, P - center);
        if (distSq > radius * radius * 4.0) continue;
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
```

### Accretion Disk Intersection

The accretion disk lies in the equatorial plane and is checked by detecting if the ray crosses this plane:

```glsl
bool CrossesEquatorialPlane(vec3 oldPos, vec3 newPos) 
{
    bool crossed = (oldPos.y * newPos.y <0.0);
    if (crossed) { diskIntersection = newPos; }
    return crossed;
}
```

## 4. Shading and Color Computation

Once an intersection is found, we compute the color of the pixel based on the object hit and relativistic effects such as gravitational redshift and Doppler shift.

```glsl
vec3 ComputeColor(Ray ray) 
{
    if (hitBlackHole) return vec3(0.0); // Black hole is black
    if (hitDisk) return SampleDiskTexture(diskIntersection); 
    if (hitObject) return ApplyLighting(ray, objectColor, hitCenter, hitRadius);
    
    return SampleBackground(ray); // Background stars, etc.
}
````

This ensures that each pixel reflects both the geometrical position and relativistic effects along the ray.

## 5. Rendering Loop

Finally, the main rendering loop iterates over every pixel on the screen, traces a ray, and stores the computed color:

```glsl
for (int y = 0; y < HEIGHT; ++y)
{
    for (int x = 0; x < WIDTH; ++x)
    {
        Ray ray = InitRayForPixel(x, y);
        TraceRay(ray);
        vec3 color = ComputeColor(ray);
        framebuffer[y*WIDTH + x] = vec4(color, 1.0);
    }
}
```