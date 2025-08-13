#type compute
#version 430 core

layout(local_size_x = 256) in;

struct Particle
{
    vec4 position;    // xyz = position, w = lifetime
    vec4 velocity;    // xyz = velocity, w = mass
    vec4 color;       // rgba = color
};

layout(std430, binding = 0) buffer ParticleBuffer
{
    Particle particles[];
} particleBuffer;

uniform float u_DeltaTime;
uniform vec3 u_Gravity;
uniform vec3 u_EmitterPosition;
uniform float u_EmissionRate;
uniform float u_ParticleLifetime;
uniform float u_Time;

void main()
{
    uint index = gl_GlobalInvocationID.x;
    
    if (index >= particles.length())
        return;
    
    Particle particle = particleBuffer.particles[index];
    
    particle.position.w -= u_DeltaTime;
    
    if (particle.position.w <= 0.0)
    {
        particle.position.xyz = u_EmitterPosition;
        particle.position.w = u_ParticleLifetime;
        
        float angle = fract(sin(dot(vec2(index, u_Time), vec2(12.9898, 78.233))) * 43758.5453) * 2.0 * 3.14159;
        float speed = 2.0 + fract(sin(dot(vec2(index + 1, u_Time), vec2(12.9898, 78.233))) * 43758.5453) * 3.0;
        particle.velocity.xyz = vec3(cos(angle) * speed, 5.0, sin(angle) * speed);
        particle.velocity.w = 1.0;
        
        particle.color = vec4(1.0, 0.5 + 0.5 * fract(sin(index * 123.456) * 43758.5453), 0.2, 1.0);
    }
    else
    {
        particle.velocity.xyz += u_Gravity * u_DeltaTime;
        particle.position.xyz += particle.velocity.xyz * u_DeltaTime;
        
        float lifeRatio = particle.position.w / u_ParticleLifetime;
        particle.color.a = lifeRatio;
    }
    
    particleBuffer.particles[index] = particle;
}
