#ifndef SHADER_TYPES_H
#define SHADER_TYPES_H

#define Color vec4

struct DirectionalLight
{
	vec4 worldDirection;
    vec4 viewDirection;

    Color diffuseColor;
    Color ambientColor;
    Color specularColor;

	// x = softness, yzw = unused
	vec4 softness;
};
#endif // SHADER_TYPES_H
