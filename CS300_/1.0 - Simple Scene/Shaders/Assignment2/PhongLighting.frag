#version 410 core

in VS_GS_VERTEX
{
    vec4 fragColor;
} vs_in;

out vec4 fragColor;

void main()
{
    fragColor = vs_in.fragColor;
}