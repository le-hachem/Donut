#type vertex
#version 330 core
layout(location = 0) in vec2 a_Pos;
layout(location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main()
{
    gl_Position = vec4(a_Pos.xy, 0.0, 1.0);
    v_TexCoord = a_TexCoord;
}

#type fragment
#version 330 core
out vec4 FragColor;

in vec2 v_TexCoord;

uniform sampler2D screenTexture;

void main()
{
    FragColor = texture(screenTexture, v_TexCoord);
}
