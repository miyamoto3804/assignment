#version 150 core
uniform mat4 MVP;
// Input vertex data, different for all executions of this shader.
in vec4 position;
in vec4 color;
out vec4 vertex_color;
void main()
{
 vertex_color = color;
 gl_Position = MVP * position;
}