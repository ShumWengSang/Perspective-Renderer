#version 410 core

uniform mat4 model;
uniform mat4 View;
uniform mat4 Projection;

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec4 vvertexNormal;

out VS_GS_VERTEX
{
    vec3 fragNormal;
    vec4 worldPosition;
    vec4 objectColor;
} vs_out;

void main()
{
    mat3 normalTransform = mat3(transpose(inverse(model)));
    vs_out.fragNormal = normalize(mat3(normalTransform) * vvertexNormal.xyz);

    vec4 fragPosition = Projection * View * model * vPosition;
    vs_out.worldPosition = fragPosition;

    vs_out.objectColor = vColor;
    gl_Position = fragPosition;
}
