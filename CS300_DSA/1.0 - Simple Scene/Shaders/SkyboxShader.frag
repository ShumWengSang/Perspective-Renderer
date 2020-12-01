#version 450 core

layout (location = 0) out vec4 fragColor;

layout (location = 0) in v_in
{
    vec3 pos;
} i;

layout (binding = 0) uniform sampler2D cubeTexture[6];

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

vec3 SampleCubeMap(vec3 vec)
{
    int index = 0;
    vec2 uv = ConvertXYZtoUV(vec, index);
     return texture(cubeTexture[index], uv).rgb;
    //return vec3(uv, float(index));
}

void main()
{
    vec3 color = SampleCubeMap(i.pos);
    fragColor = vec4(color, 1.0);
}
