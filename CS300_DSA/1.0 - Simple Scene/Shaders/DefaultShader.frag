#version 450 core

layout (location = 0) out vec4 fragColor;

layout (location = 0) in v_in
{
    vec3 col;
} i;

void main()
{
    fragColor = vec4(i.col, 1.0);
}
