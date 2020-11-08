#version 450 core
layout (location = 0) out vec4 fragColor;

layout (location = 0) in v_in
{
    vec3 fragColor;
    vec3 ambient;
    vec3 specular;
    vec3 diffuse;
    vec3 localK;
} i;

void main()
{
    fragColor = vec4(clamp(i.fragColor, 0, 1), 1.0);
    //fragColor = vec3(1.0, 1.0, 1.0, 1.0f);
}