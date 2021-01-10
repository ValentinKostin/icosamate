#version 330 core

uniform mat4 modelViewProjectionMatrix;

in vec3 position;
in vec4 color;

out vec4 fragColor;

void main(void)
{
	gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);
	fragColor = color;
}
