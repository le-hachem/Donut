#type vertex

#version 330 core

layout (location = 0) in vec2 a_Pos;
layout (location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main() 
{
    gl_Position = vec4(a_Pos, 0.0, 1.0);
    v_TexCoord = a_TexCoord;
}

#type fragment

#version 330 core

in  vec2 v_TexCoord;
out vec4 o_FragColor;

uniform sampler2D u_ScreenTexture;

void main() 
{
    o_FragColor = texture(u_ScreenTexture, v_TexCoord);
}
