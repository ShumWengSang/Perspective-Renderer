#version 450
layout (location = 0) out vec4 fragColor;

layout (location = 0) in v_in
{
    vec3 vPosition;
    vec3 vNormal;
} i;

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

void main() {
    // Light position in view space
    const vec3 lightPos = (View * lightPos[0]).xyz;
    const vec3 vNormal = normalize(i.vNormal);

    // Calculate light color
    // Local Ambient
    const vec3 ambient = coEfficients.r * AmbientColor;

    // Local Diffuse
    // Calc light direction
    const vec3 lightDir = normalize(lightPos - i.vPosition);
    const float N_dot_L = max( dot( vNormal, lightDir), 0.0f);
    const vec3 diffuse = N_dot_L * diffuseColor_kd * coEfficients.g;

    // Specular
    // I_s * K_s * max( R dot V, 0) ^ ns
    const vec3 EyePosition = (View * CameraPosition).xyz;
    const vec3 R = reflection(-lightDir, vNormal);
    //const vec3 R = normalize(reflect(-lightDir, vNormal));
    const vec3 V = normalize(EyePosition - i.vPosition);
    const vec3 specular = coEfficients.b * specularColor_ks * pow(max(dot(R,V), 0), 32);

    // Emissive
    vec3 emissive = EmissiveColor;

    // Local Color
    vec3 localLightColor = ambient + diffuse + specular;
    // fragColor = vec4(localLightColor, 1.0);

//    fragColor = vec4(vec3(max(dot(R,V), 0)), 1.0);
//    fragColor = vec4(specular + diffuse, 1.0f);
    // Find attenuation
    float distance = distance(lightPos, i.vPosition);
    float att = attenuation(globalValues.y, globalValues.z, globalValues.w, distance);

    // Find the S value
    float s = Svalue(NearFar.x, NearFar.y, length(V));

    vec3 ILocal = emissive + globalAmbientColor.xyz * coEfficients.r + att * (localLightColor);
    vec3 Ifinal = s * ILocal + (1 - s) * fogColor.xyz;

    fragColor = vec4(Ifinal, 1.0);
}
