#type vertex

#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec3 v_Normal;
out vec3 v_WorldPos;

void main()
{
    v_WorldPos = vec3(u_Transform * vec4(a_Position, 1.0));
    v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
    gl_Position = u_ViewProjection * vec4(v_WorldPos, 1.0);
}

#type fragment

#version 330 core

in vec3 v_Normal;
in vec3 v_WorldPos;

uniform vec3  u_Color;
uniform float u_Specular;
uniform float u_Emission;
uniform vec3  u_LightPos;
uniform vec3  u_CameraPos;

out vec4 o_FragColor;

void main()
{
    vec3 normal     = normalize(v_Normal);
    vec3 lightDir   = normalize(u_LightPos - v_WorldPos);
    vec3 viewDir    = normalize(u_CameraPos - v_WorldPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    
    vec3 ambient  = u_Color * 0.1;
    vec3 diffuse  = u_Color * diff;
    vec3 specular = vec3(u_Specular) * spec;
    vec3 emission = u_Color * u_Emission;
    
    vec3 result = ambient + diffuse + specular + emission;
    o_FragColor = vec4(result, 1.0);
}
