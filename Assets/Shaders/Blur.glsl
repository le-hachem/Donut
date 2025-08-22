#type vertex

#version 330 core

layout (location = 0) in vec2 a_Pos;
layout (location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main() 
{
    gl_Position = vec4(a_Pos, 0.0, 1.0);
    v_TexCoord  = a_TexCoord;
}

#type fragment

#version 330 core

in  vec2 v_TexCoord;
out vec4 o_FragColor;

uniform sampler2D u_ScreenTexture;
uniform vec2      u_Resolution;
uniform float     u_BlurStrength;
uniform float     u_GlowIntensity;

void main() 
{
    vec2 texelSize = 1.0 / u_Resolution;
    vec4 centerColor = texture(u_ScreenTexture, v_TexCoord);
    
    float brightness = (centerColor.r + centerColor.g + centerColor.b) / 3.0;
    float glowMask   = smoothstep(0.05, 0.3, brightness);
    
    vec4 blurredColor = vec4(0.0);
    float totalWeight = 0.0;
    
    for (int x = -8; x <= 8; x++) 
    {
        for (int y = -8; y <= 8; y++) 
        {
            vec2 offset = vec2(x, y) * texelSize * u_BlurStrength;
            vec4 sampleColor = texture(u_ScreenTexture, v_TexCoord + offset);
            
            float weight = exp(-(x*x + y*y) / (2.0 * 8.0 * 8.0));
            blurredColor += sampleColor * weight;
            totalWeight += weight;
        }
    }
    
    blurredColor /= totalWeight;
    
    vec3 emissionColor = vec3(1.0, 0.8, 0.6);
    vec3 glowColor     = emissionColor * glowMask * u_GlowIntensity * 2.0;
    vec3 auraColor     = blurredColor.rgb * glowMask * u_GlowIntensity * 0.8;
    vec3 finalColor    = centerColor.rgb + glowColor + auraColor;
    
    o_FragColor = vec4(finalColor, centerColor.a);
}
