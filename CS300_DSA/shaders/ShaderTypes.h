#ifndef SHADER_TYPES_H
#define SHADER_TYPES_H

// color.rgb is the color [0, 1] and color.a is [0, 1] but maps to a [0, 63] exponential scale y=2^(6x)-1
#define Color vec4

struct DirectionalLight
{
	vec4 worldDirection;
    vec4 viewDirecion;

    Color color;

	// x = softness, yzw = unused
	vec4 softness;
};

#endif // SHADER_TYPES_H
