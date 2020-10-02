#version 410 core

uniform mat4 model;
uniform mat4 viewProjection;

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec4 vvertexNormal;

out VS_OUT
{
    vec3 fragNormal;
    vec4 worldPosition;
    vec4 objectColor;
} vs_out;

void main()
{
    mat3 normalTransform = mat3(transpose(inverse(model)));
    vs_out.fragNormal = mat3(normalTransform) * vvertexNormal.xyz;

    vec4 fragPosition = viewProjection * model * vPosition;
    vs_out.worldPosition = fragPosition;

    vs_out.objectColor = vColor;
    gl_Position = fragPosition;
}
