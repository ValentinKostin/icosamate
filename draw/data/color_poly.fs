#version 330 core

// входящие параметры, которые получены из вершинного шейдера
in vec4 fragColor;

void main(void)
{
	// получим цвет пикселя из текстуры по текстурным координатам
	gl_FragColor = fragColor;
}
