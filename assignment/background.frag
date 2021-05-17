#version 330 core

// Values that stay constant for the whole mesh.
uniform sampler2DRect image;

layout (location = 0) out vec4 fragment;

void main()
{
	fragment = texture(image, gl_FragCoord.xy);
}