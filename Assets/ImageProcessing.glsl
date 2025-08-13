#type compute
#version 430 core

layout(local_size_x = 16, local_size_y = 16) in;

layout(binding = 0) uniform sampler2D u_InputTexture;
layout(binding = 1, rgba8) uniform image2D u_OutputTexture;

uniform vec2 u_TextureSize;
uniform float u_Brightness;
uniform float u_Contrast;
uniform float u_Saturation;

void main()
{
    ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
    
    if (pixelCoord.x >= int(u_TextureSize.x) || pixelCoord.y >= int(u_TextureSize.y))
        return;
    
    vec2 texCoord = (vec2(pixelCoord) + 0.5) / u_TextureSize;
    vec4 color = texture(u_InputTexture, texCoord);
    
    color.rgb = (color.rgb - 0.5) * u_Contrast + 0.5 + u_Brightness;
    
    float luminance = dot(color.rgb, vec3(0.299, 0.587, 0.114));
    color.rgb = mix(vec3(luminance), color.rgb, u_Saturation);
    color.rgb = clamp(color.rgb, 0.0, 1.0);
    
    imageStore(u_OutputTexture, pixelCoord, color);
}
