#type vertex

#version 330 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
uniform float u_GridSize;

void main()
{
    // Scale the grid based on the grid size uniform
    vec3 scaledPosition = a_Position * (u_GridSize / 50.0);
    gl_Position = u_ViewProjection * u_Transform * vec4(scaledPosition, 1.0);
}

#type fragment

#version 330 core

uniform vec3  u_GridColor;
uniform float u_GridAlpha;
uniform float u_GridSize;
uniform vec3  u_CameraPos;

out vec4 o_FragColor;

void main()
{
    float distance  = length(u_CameraPos);
    float fadeFactor = 1.0 - clamp(distance / 100.0, 0.0, 0.8);
    
    vec3 color  = u_GridColor;
    float alpha = u_GridAlpha * fadeFactor;
    
    float gridFade = 1.0 - clamp(distance / 50.0, 0.0, 0.9);
    alpha *= gridFade;
    
    o_FragColor = vec4(color, alpha);
}
