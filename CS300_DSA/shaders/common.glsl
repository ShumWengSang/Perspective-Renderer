#ifndef COMMON_GLSL
#define COMMON_GLSL

#include <ShaderTypes.h>

#define PI     (3.14159265358979323846)
#define TWO_PI (2.0 * PI)

// color.rgb is the color [0, 1] and color.a is [0, 1] but maps to a [0, 63] exponential scale y=2^(6x)-1
// see shader_types.h for definitions
vec3 rgbFromColor(Color color)
{
    float x = color.a;
    float y = pow(2.0, 5.0 * x) - 1.0;
    return vec3(color.rgb * y);
}

float lengthSquared(vec2 v)
{
    return dot(v, v);
}

float lengthSquared(vec3 v)
{
    return dot(v, v);
}

float square(float x)
{
    return x * x;
}

float saturate(float x)
{
    return clamp(x, 0.0, 1.0);
}

// Source: http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec2 sphericalUvFromDirection(vec3 direction)
{
    float phi = atan(direction.z, direction.x);
    float theta = acos(clamp(direction.y, -1.0, +1.0));

    if (phi < 0.0) phi += TWO_PI;
    return vec2(phi / TWO_PI, theta / PI);
}

#endif // COMMON_GLSL
