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
layout(location = 3) uniform float RefractiveIndex;
layout(location = 4) uniform float LightMixRatio;
layout(location = 5) uniform int Mode;
layout(location = 6) uniform bool toMixLights;

layout(binding = 0, std140) uniform UBO
{
    mat4 Projection;
    mat4 View;
    vec4 NearFar; // Z and W are nothing
    ivec4 Modes;
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
layout (binding = 2) uniform sampler2D cubeTexture[6];

vec2 ConvertXYZtoUV(vec3 xyz, out int index)
{
    float u, v;
    float absX = abs(xyz.x);
    float absY = abs(xyz.y);
    float absZ = abs(xyz.z);

    bool isXPositive = xyz.x > 0 ? true : false;
    bool isYPositive = xyz.y > 0 ? true : false;
    bool isZPositive = xyz.z > 0 ? true : false;

    float maxAxis, uc, vc;

    // POSITIVE X
    if (isXPositive && absX >= absY && absX >= absZ) {
        // u (0 to 1) goes from +z to -z
        // v (0 to 1) goes from -y to +y
        maxAxis = absX;
        uc = -xyz.z;
        vc = -xyz.y;
        index = 0;
    }
    // NEGATIVE X
    if (!isXPositive && absX >= absY && absX >= absZ) {
        // u (0 to 1) goes from -z to +z
        // v (0 to 1) goes from -y to +y
        maxAxis = absX;
        uc = xyz.z;
        vc = -xyz.y;
        index = 1;
    }
    // POSITIVE Y
    if (isYPositive && absY >= absX && absY >= absZ) {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from +z to -z
        maxAxis = absY;
        uc = xyz.x;
        vc = xyz.z;
        index = 2;
    }
    // NEGATIVE Y
    if (!isYPositive && absY >= absX && absY >= absZ) {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from -z to +z
        maxAxis = absY;
        uc = xyz.x;
        vc = -xyz.z;
        index = 3;
    }
    // POSITIVE Z
    if (isZPositive && absZ >= absX && absZ >= absY) {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from -y to +y
        maxAxis = absZ;
        uc = xyz.x;
        vc = -xyz.y;
        index = 4;
    }
    // NEGATIVE Z
    if (!isZPositive && absZ >= absX && absZ >= absY) {
        // u (0 to 1) goes from +x to -x
        // v (0 to 1) goes from -y to +y
        maxAxis = absZ;
        uc = -xyz.x;
        vc = -xyz.y;
        index = 5;
    }

    // Convert range from -1 to 1 to 0 to 1
    u = 0.5f * (uc / maxAxis + 1.0f);
    v = 0.5f * (vc / maxAxis + 1.0f);
    return vec2(u,v);
}

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

vec3 refraction(vec3 incident, vec3 normal, float ratio)
{
    float k = 1.0 - ratio * ratio * (1.0 - dot(normal, incident) * dot(normal, incident));
    if(k < 0.0)
        return vec3(0.0);
    else
        return ratio * incident - (ratio * dot(normal, incident) + sqrt(k)) * normal;
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
    //lightAmbientColor[lightID].xyz
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
    const vec3 V = normalize(EyePosition - i.vPosition);
    const vec3 H = normalize(LocalLightDir + V);
    const vec3 specular = coEfficients.b * specularColor_ks * pow(max(dot(H, vNormal), 0), specularColor_ks.r *
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

vec3 SampleColorsCubeMap(vec3 R)
{
    int index = 0;
    vec2 uv = ConvertXYZtoUV(normalize(R), index);
     return texture(cubeTexture[index], uv).rgb;
}

vec3 ReflectionEnvironment(vec3 EyePosition)
{
    const vec3 I = normalize(i.vPosition - EyePosition);
    const vec3 Normal = normalize(i.vNormal);
    const vec3 R = normalize(reflection(I, Normal));
    return SampleColorsCubeMap(R);
}

vec3 RefractionEnvironment(vec3 EyePosition, float ratio)
{
    const vec3 I = normalize(i.vPosition - EyePosition);
    const vec3 Normal = normalize(i.vNormal);
    vec3 R = vec3(1.0);
    // Split the refraction into three bins.
    vec3 Red = refraction(I, Normal, ratio + ratio * 0.05f);
    vec3 Green = refraction(I, Normal, ratio );
    vec3 Blue = refraction(I, Normal, ratio - ratio * 0.05f);

    //    R.y = -R.y;
    Red.x = -Red.x, Green.x = -Green.x, Blue.x = -Blue.x;
    //R.z = -R.z;

    R.r = SampleColorsCubeMap(Red).r;
    R.g = SampleColorsCubeMap(Green).g;
    R.b = SampleColorsCubeMap(Blue).b;

    return R;

}

void main() {


    const vec3 emissive = EmissiveColor;
    vec3 localLightColor;
    // Local Color
    for(int i = 0; i < int(globalValues.x); ++i)
    {
        localLightColor += CalcLight(i);
    }

    // Find the local color
    const vec3 EyePosition = (View * CameraPosition).xyz;
    const vec3 V = normalize(EyePosition - i.vPosition);
    const float s = Svalue(NearFar.x, NearFar.y, length(V));
    const vec3 ILocal = emissive + globalAmbientColor.xyz * coEfficients.r + localLightColor;
    const vec3 ColorFinalColor = s * ILocal + (1 - s) * fogColor.xyz;


    // Fresnal
    const vec3 I = normalize(i.vPosition - EyePosition);
    const vec3 Normal = normalize(i.vNormal);
    const float Eta = 1.0f / RefractiveIndex; // Ratio of indices of refraction
    const float FresnelPower = 5.0;
    const float F = ((1.0-Eta) * (1.0-Eta)) / ((1.0+Eta) * (1.0+Eta));
    const float Ratio = F + (1.0 - F) * pow((1.0 - dot(-I, Normal)), FresnelPower);

    const vec3 RefractedColors = RefractionEnvironment(EyePosition, Eta);
    const vec3 ReflectedColors = ReflectionEnvironment(EyePosition);

    // Mix the refracted and reflected together
    const vec3 environmentColor = mix(RefractedColors, ReflectedColors, Ratio);

    // Only reflection
    if(Mode == 0)
    {
        fragColor = vec4(ReflectedColors, 1.0);
    }
    else if(Mode == 1) // Only refraction
    {
        fragColor = vec4(RefractedColors, 1.0);
    }
    else if(Mode == 2) // Mixed Mode
    {
        fragColor = vec4(environmentColor, 1.0);
    }

    if(toMixLights)
    {
        const vec3 lightAndEnvironmentColor = mix(fragColor.xyz, ColorFinalColor, LightMixRatio);
        fragColor = vec4(lightAndEnvironmentColor, 1.0);
    }
}
