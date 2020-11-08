#version 450 core
out gl_PerVertex { vec4 gl_Position; };

out VS_GS_VERTEX
{
    vec3 fragColor;
    vec3 ambient;
    vec3 specular;
    vec3 diffuse;
    vec3 localK;
} vs_out;

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec3 vvertexNormal;

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform vec3 AmbientColor;
layout(location = 2) uniform vec3 EmissiveColor;

layout(binding = 0, std140) uniform UBO
{
    mat4 Projection;
    mat4 View;
    vec4 NearFar; // Z and W are nothing
};

layout(binding = 1, std140) uniform LightUBO
{
    vec4 lightPos[16];
    vec4 lightColor[16];
    vec4 lightInfo[16]; // x = light type, y = spotlight fall off , z = theta, w = phi
    vec4 CameraPosition;
    vec4 fogColor;
    vec4 globalAmbientColor;
    vec4 coEfficients;  // r = ambient, g = diffuse, b = specular
    vec4 globalValues; // Num of Light, c1, c2, c3
};

vec3 diffuseColor_kd = vec3(0.0f, 0.5f, 0.0f);
vec3 specularColor_ks = vec3(1f, 1f, 1f);

// Assumes all are normalized
vec3 reflection(vec3 L, vec3 normal)
{
    // R = 2 (N dot L) * N - L
    float ndotl = dot( normal, L);
    return L - 2 * ndotl * normal;
}

float attenuation(float c1, float c2, float c3, float distance)
{
    return min(1 / (c1 + c2 * distance + c3 * distance * distance), 1.0);
}

float Svalue(float near, float far, float v)
{
    return (far - v) / (far - near);
}

void main()
{
    // Calculate fragment position
    const vec4 fragPosition = Projection * View * model * vec4(vPosition, 1.0);
    const vec4 worldPos = model * vec4(vPosition, 1.0);
    gl_Position = fragPosition;

    // Calculate Normal
    const mat3 normalTransform = mat3(transpose(inverse(model)));
    const vec3 fragNormal = normalize(normalTransform * normalize(vvertexNormal.xyz));

    // Calculate light color
    // Local Ambient
    const vec3 ambient = coEfficients.r * AmbientColor;

    // Local Diffuse
    // Calc light direction
    const vec3 lightDir = normalize((lightPos[0] - worldPos).xyz);
    const float N_dot_L = max( dot( fragNormal, lightDir), 0.0f);
    const vec3 diffuse = N_dot_L * diffuseColor_kd * coEfficients.g;

    // Specular
    // I_s * K_s * max( R dot V, 0) ^ ns
    vec3 R = reflection(-lightDir, fragNormal);
    // vec3 R = reflect(-lightDir, fragNormal);
    vec3 V = normalize(CameraPosition.xyz - worldPos.xyz);
    vec3 specular = coEfficients.b * specularColor_ks * pow(max(dot(R,V), 0), 32);
    vs_out.ambient = lightPos[0].xyz;
    vs_out.diffuse = worldPos.xyz;
    vs_out.specular = fragNormal.xyz;

    // Emissive
    vec3 emissive = EmissiveColor;

    // Local Color
    vec3 localLightColor = ambient + diffuse + specular;
    vs_out.localK = lightDir;

    vs_out.fragColor = diffuse + ambient + specular;

    // Find attenuation
    float distance = distance(lightPos[0], worldPos);
    float att = attenuation(globalValues.y, globalValues.z, globalValues.w, distance);

    // Find the S value
    float s = Svalue(NearFar.x, NearFar.y, length(V));

    vec3 ILocal = emissive + globalAmbientColor.xyz * coEfficients.r + att * (diffuse + specular);
    vec3 Ifinal = s * ILocal + (1 - s) * fogColor.xyz;
    // vs_out.fragColor = Ifinal;
}
