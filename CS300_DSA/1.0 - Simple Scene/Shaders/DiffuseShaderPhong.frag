#version 450 core
layout(location = 0) out vec4 fragColor;

layout(location = 0)in v_in
{
    vec4 worldPosition;
    vec3 fragNormal;
    vec3 objectColor;
} i;

layout(location = 0) uniform vec3 lightColor;
layout(location = 1) uniform vec3 lightDirection;

void main()
{
    // Ambient
    float ambientCoefficient = 0.9f;
    vec4 ambient = vec4(ambientCoefficient * lightColor, 1.0f);

    // Diffuse
    vec3 lightDir = normalize(lightDirection);
    float N_dot_L = max( dot( normalize(i.fragNormal), lightDir), 0.0f);
    vec4 diffuse = vec4(N_dot_L * lightColor, 1.0f);

    // Final Color
    vec4 finalLightColor = ambient + diffuse;
    fragColor = finalLightColor * vec4(i.objectColor, 1.0);

}