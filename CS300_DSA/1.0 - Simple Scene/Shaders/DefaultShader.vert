#version 450 core
out gl_PerVertex { vec4 gl_Position; };

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec3 vvertexNormal;
layout(location = 3) in vec2 vTexCoord;

layout (location = 0) out v_out
{
    vec3 col;
} o;

layout(location = 0) uniform mat4 model;

layout(binding = 0, std140) uniform UBO
{
    mat4 Projection;
    mat4 View;
};

void main()
{
    o.col = vColor;
    gl_Position = Projection * View * model * vec4(vPosition, 1.0);
}