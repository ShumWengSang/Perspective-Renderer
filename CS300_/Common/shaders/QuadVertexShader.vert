#version 410 core

uniform mat4  vertexTransform;

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4  vPosition;
layout(location = 1) in vec4 vColor;

out vec4 fcolor;

void main()
{
    fcolor = vColor;
    vec4 vPos = vPosition;
    gl_Position = vertexTransform * vPos;
}
