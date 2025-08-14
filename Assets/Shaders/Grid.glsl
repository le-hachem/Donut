#type vertex
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 viewProj;

void main()
{
    gl_Position = viewProj * vec4(aPos, 1.0);
}

#type fragment
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(0.5, 0.5, 0.5, 0.3);
}
