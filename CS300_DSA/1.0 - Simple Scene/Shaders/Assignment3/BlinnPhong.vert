#version 450

// We are not using this file as it is the same as the PhongShading.vert.
#if 0
out gl_PerVertex { vec4 gl_Position; };

out VS_GS_VERTEX
{
    vec3 vPosition;
    vec3 viewPosition;
    vec3 vNormal;
    vec2 vTexCoord;
} vs_out;

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec3 vvertexNormal;
layout(location = 3) in vec2 TexCoord;

layout(location = 0) uniform mat4 model;

layout(binding = 0, std140) uniform UBO
{
    mat4 Projection;
    mat4 View;
    vec4 NearFar; // Z and W are nothing
    ivec4 Modes; // X = GPU mode
};

void main() {
    // Calculate view space attributes
    const mat4 ModelViewMatrix = View * model;
    const mat3 normalTransform = transpose(inverse(mat3(ModelViewMatrix)));

    vs_out.vNormal = normalize(normalTransform * normalize(vvertexNormal));
    vs_out.viewPosition = (ModelViewMatrix * vec4(vPosition, 1.0)).xyz;
    vs_out.vTexCoord = TexCoord;
    vs_out.vPosition = vPosition;

    gl_Position = Projection * ModelViewMatrix * vec4(vPosition, 1.0);;
}
#endif