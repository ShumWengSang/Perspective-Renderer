#version 410 core

uniform mat4 model;
uniform mat4 View;

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec4 vvertexNormal;

out VS_GS_VERTEX
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
