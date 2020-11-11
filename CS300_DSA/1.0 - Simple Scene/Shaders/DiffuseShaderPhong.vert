#version 450 core
out gl_PerVertex { vec4 gl_Position; };
layout(location = 0) out v_out
{
    vec4 worldPosition;
    vec3 fragNormal;
} o;

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec3 vvertexNormal;

layout(location = 0) uniform mat4 model;

layout(binding = 0, std140) uniform UBO
{
    mat4 Projection;
    mat4 View;
    vec4 NearFar; // Z and W are nothing
    ivec4 Modes; // X = GPU mode
};


void main()
{
    mat3 normalTransform = mat3(transpose(inverse(model)));
    o.fragNormal = normalize(mat3(normalTransform) * vvertexNormal.xyz);

    vec4 fragPosition = Projection * View * model * vec4(vPosition, 1.0);
    o.worldPosition = fragPosition;

    gl_Position = fragPosition;
}
