#version 450 core
layout(location = 0) out vec4 fragColor;

layout(location = 0) uniform vec4 lineColor;

void main()
{
    fragColor = lineColor;
}