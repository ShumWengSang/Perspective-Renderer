#version 460
#include <ShaderLocations.h>
PredefinedOutput(vec4, o_color);
uniform vec4 u_color;
void main()
{
    o_color = vec4(0, 1, 0, 1);
}
