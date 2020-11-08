#version 450
out gl_PerVertex { vec4 gl_Position; };

out VS_GS_VERTEX
{
    vec3 viewPosition;
    vec3 vNormal;
} vs_out;

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec3 vvertexNormal;

layout(location = 0) uniform mat4 model;

layout(binding = 0, std140) uniform UBO
{
    mat4 Projection;
    mat4 View;
    vec4 NearFar; // Z and W are nothing
};

void main() {
    // Calculate view space attributes
    const mat4 ModelViewMatrix = View * model;
    const mat3 normalTransform = transpose(inverse(mat3(ModelViewMatrix)));

    vs_out.vNormal = normalize(normalTransform * normalize(vvertexNormal));
    vs_out.viewPosition = (ModelViewMatrix * vec4(vPosition, 1.0)).xyz;

    gl_Position = Projection * ModelViewMatrix * vec4(vPosition, 1.0);;
}
