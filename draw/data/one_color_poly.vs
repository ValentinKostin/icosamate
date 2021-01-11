#version 330 core

uniform mat4 modelViewProjectionMatrix;
uniform vec4 sketchColor;

out vec4 fragColor;

in vec3 position;

void main(void)
{
	gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);
	fragColor = sketchColor;
}
