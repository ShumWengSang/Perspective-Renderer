#version 450
layout (location = 0) out vec4 fragColor;

layout (location = 0) in v_in
{
    vec3 oPosition;
    vec3 vPosition;
    vec3 vNormal;
    vec2 vTexCoord;
} i;

layout(location = 1) uniform vec3 AmbientColor;
layout(location = 2) uniform vec3 EmissiveColor;

layout(binding = 0, std140) uniform UBO
{
    mat4 Projection;
    mat4 View;
    vec4 NearFar; // Z and W are nothing
    ivec4 Modes; // x = GPU mode
};

layout(binding = 1, std140) uniform LightUBO
{
    vec4 lightPos[16];
    vec4 lightDiffuseColor[16];
    vec4 lightAmbientColor [16];
    vec4 lightSpecularColor [16];
    vec4 lightInfo[16]; // x = light type, y = spotlight fall off , z = theta, w = phi
    vec4 lightDir[16];
    vec4 CameraPosition;
    vec4 fogColor;
    vec4 globalAmbientColor;
    vec4 coEfficients;  // r = ambient, g = diffuse, b = specular
    vec4 globalValues; // Num of Light, c1, c2, c3
};

layout (binding = 0) uniform sampler2D tex_Diffuse;
layout (binding = 1) uniform sampler2D tex_Specular;

vec2 GenerateUV(vec3 vertex, int mode, float max, float min)
{
    const float PI = 3.14159f;
    // Planar
    if(mode == 1)
    {
        float u = (vertex.x + min) / (max - min);
        float v = (vertex.y + min) / (max - min);
        return vec2(u,v);
    }
    // Cylinder
    else if(mode == 2)
    {

        const float theta = atan(vertex.y, vertex.x);
        float z = (vertex.z + min) / (max - min);
        const float u = theta / (2 * PI);
        const float v = z;
        return vec2(u,v);
    }
    // Spherical
    else if(mode == 3)
    {
        float theta = atan(vertex.y, vertex.x);
        float r = vertex.x * vertex.x + vertex.y * vertex.y + vertex.z * vertex.z;
        r = sqrt(r);
        float pheta = acos(vertex.z / r);
        float u = theta / (2 * PI);
        float v = (PI - pheta) / PI;
        return vec2( u, v);
    }
    else
    return vec2(0,0);
}

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

float CalcSpotlight(int lightID)
{
    const vec3 lightPos = (View * lightPos[lightID]).xyz;
    const vec3 L = normalize(lightDir[lightID].xyz);
    const vec3 D = normalize(i.vPosition - lightPos);

    const float phi = lightInfo[lightID].w;
    const float theta = lightInfo[lightID].z;
    const float fallOut = lightInfo[lightID].y;
    // If vertex is in inner cone, treat as no light
    if(dot(L,D) < cos(phi))
    {
        return 0.0f;
    }
    else if(dot(L,D) > cos(theta))    // Treat as point light
    {
        return 1.0f;
    }
    else    // Inside the both cones, calc intensity
    {
        const float alpha = dot(L,D) / (length(L) * length(D));
        return pow((alpha - cos(phi)) / (cos(theta) - cos(phi)), fallOut);
    }
}

vec3 CalcLight(int lightID)
{
    const int UVGenerateMode = Modes.x;
    const int UseNormalUV = Modes.y;
    vec2 generatedTexCoord;
    if(UseNormalUV == 0)
    {
        generatedTexCoord = GenerateUV(i.oPosition, UVGenerateMode, -1.0f, 1.0f);
    }
    else
    {// Using normals to generate
        generatedTexCoord = GenerateUV(normalize(i.vNormal), UVGenerateMode, 0.0f, 1.0f);
    }
    const vec2 texCoord = UVGenerateMode == 0 ? i.vTexCoord :generatedTexCoord;

    const vec3 diffuseColor_kd = texture(tex_Diffuse, texCoord).xyz;
    const vec3 specularColor_ks = texture(tex_Specular, texCoord).xyz;

    // Light position in view space
    const vec3 lightPos = (View * lightPos[lightID]).xyz;
    const vec3 vNormal = normalize(i.vNormal);

    // Calculate light color
    // Local Ambient
    const vec3 ambient = coEfficients.r * lightAmbientColor[lightID].xyz;
    const float lightMode = lightInfo[lightID].x;

    // Local Diffuse
    // Calc light direction: Use light dir if directional light
    vec3 LocalLightDir;
    if(lightMode == 0.0) // Point light
        LocalLightDir = normalize(lightPos - i.vPosition);
    else // Directional light or spotlight
        LocalLightDir = -lightDir[lightID].xyz;
    const float N_dot_L = max( dot( vNormal, LocalLightDir), 0.0f);
    const vec3 diffuse = N_dot_L * diffuseColor_kd * coEfficients.g;

    // Specular
    // I_s * K_s * max( R dot V, 0) ^ ns
    const vec3 EyePosition = (View * CameraPosition).xyz;
    const vec3 R = reflection(-LocalLightDir, vNormal);
    const vec3 V = normalize(EyePosition - i.vPosition);
    const vec3 specular = coEfficients.b * specularColor_ks * pow(max(dot(R,V), 0), specularColor_ks.r *
    specularColor_ks.r * 255);

    // Find attenuation
    const float distance = distance(lightPos, i.vPosition);
    const float att = attenuation(globalValues.y, globalValues.z, globalValues.w, distance);

    // If we have spotlight
    float spotlightEffect =  1.0;
    if(lightMode == 2.0) // If we are spotlight
        spotlightEffect = CalcSpotlight(lightID);
    return att * ambient + att * spotlightEffect * (diffuse * lightDiffuseColor[lightID].xyz + specular *
    lightSpecularColor[lightID].xyz);
}

void main() {
    // Emissive
    const vec3 emissive = EmissiveColor;
    vec3 localLightColor;
    // Local Color
    for(int i = 0; i < int(globalValues.x); ++i)
    {
        localLightColor += CalcLight(i);
    }

    // Find the S value
    const vec3 EyePosition = (View * CameraPosition).xyz;
    const vec3 V = normalize(EyePosition - i.vPosition);
    const float s = Svalue(NearFar.x, NearFar.y, length(V));

    const vec3 ILocal = emissive + globalAmbientColor.xyz * coEfficients.r + localLightColor;
    const vec3 Ifinal = s * ILocal + (1 - s) * fogColor.xyz;

    fragColor = vec4(Ifinal, 1.0);
}
