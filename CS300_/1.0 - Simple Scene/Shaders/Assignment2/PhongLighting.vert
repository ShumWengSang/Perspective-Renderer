#version 410 core

uniform mat4 model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 lightColor;
uniform vec3 lightDirection;

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec4 vvertexNormal;

out VS_GS_VERTEX
{
    vec4 fragColor;
} vs_out;

void main()
{
    mat3 normalTransform = mat3(transpose(inverse(model)));
    vec3 fragNormal = normalize(mat3(normalTransform) * vvertexNormal.xyz);

    vec4 fragPosition = Projection * View * model * vPosition;
    vec4 worldPosition = fragPosition;

    // Ambient
    float ambientCoefficient = 0.9f;
    vec4 ambient = vec4(ambientCoefficient * lightColor, 1.0f);

    // Diffuse
    vec3 lightDir = normalize(lightDirection);
    float N_dot_L = max( dot( normalize(fragNormal), lightDir), 0.0f);
    vec4 diffuse = vec4(N_dot_L * lightColor, 1.0f);

    // Final Color
    vec4 finalLightColor = ambient + diffuse;

    vs_out.fragColor = finalLightColor * vColor;
    gl_Position = fragPosition;
}
