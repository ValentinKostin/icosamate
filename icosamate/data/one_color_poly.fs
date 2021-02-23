#version 330 core

out vec4 resColor;

uniform vec4 sketchColor;

void main(void)
{
	resColor = sketchColor;
}
