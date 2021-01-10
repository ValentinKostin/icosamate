#version 330 core

// матрица преобразования координат, получаемая из программы
uniform mat4 modelViewProjectionMatrix;

// входящие вершинные атрибуты
in vec3 position;
in vec4 a_texColor;


// исходящие параметры, которые будут переданы в фрагментный шейдер
out vec4 fragColor;

void main(void)
{
	// перевод позиции вершины из локальных координат в однородные
	gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);

	// передадим цвет
	fragColor = a_texColor;
}
