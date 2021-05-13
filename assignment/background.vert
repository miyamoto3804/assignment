#version 330 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;
// Input vertex data, different for all executions of this shader.

void main()
{
 gl_Position = position;
}