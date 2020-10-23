#version 410 core

uniform mat4 model;
uniform mat4 View;
uniform mat4 Projection;

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vPosition;

void main()
{
    gl_Position = Projection * View * model * vPosition;
}