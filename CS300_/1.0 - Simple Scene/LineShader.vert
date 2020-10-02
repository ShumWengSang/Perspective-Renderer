#version 410 core

uniform mat4 model;
uniform mat4 viewProjection;

layout(location = 0) in vec4 vPosition;

void main()
{
    vec4 fragPosition = viewProjection * model * vPosition;
    gl_Position = fragPosition;
}
