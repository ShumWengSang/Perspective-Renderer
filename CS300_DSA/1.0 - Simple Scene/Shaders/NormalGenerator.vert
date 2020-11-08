#version 450 core
out gl_PerVertex { vec4 gl_Position; };

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec4 vvertexNormal;

layout(location = 0) uniform mat4 model;

layout(binding = 0, std140) uniform UBO
{
    mat4 Projection;
    mat4 View;
};

layout(location = 0) out VS_GS_VERTEX
{
    vec3 fragNormal;
    vec4 color;
} vs_out;

void main()
{
    mat3 normalTransform = mat3(transpose(inverse(model)));
    vs_out.fragNormal = normalize(mat3(normalTransform) * vvertexNormal.xyz);
    vs_out.color = vColor;
    gl_Position = View * model * vPosition;
}
