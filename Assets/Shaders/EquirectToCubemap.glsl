#type vertex

#version 330 core

layout (location = 0) in vec3 a_Pos;
out vec3 v_WorldPos;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
    v_WorldPos = a_Pos;
    gl_Position = u_Projection * u_View * vec4(v_WorldPos, 1.0);
}

#type fragment

#version 330 core

out vec4 o_FragColor;
in vec3 v_WorldPos;

uniform sampler2D u_EquirectangularMap;
const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}
void main()
{
    vec2 uv = SampleSphericalMap(normalize(v_WorldPos));
    vec3 color = texture(u_EquirectangularMap, uv).rgb;
    o_FragColor = vec4(color, 1.0);
}
