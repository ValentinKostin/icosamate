#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "common.h"
#include "OpenGL.h"
#include "GLWindow.h"
#include "Shader.h"
#include "Texture.h"
#include "draw.h"
#include "draw_impl.h"

// построение перспективной матрицы
static void Matrix4Perspective(Matrix4 M, float fovy, float aspect, float znear, float zfar)
{
	float f = 1 / tanf(fovy / 2),
	      A = (zfar + znear) / (znear - zfar),
	      B = (2 * zfar * znear) / (znear - zfar);

	M[ 0] = f / aspect; M[ 1] =  0; M[ 2] =  0; M[ 3] =  0;
	M[ 4] = 0;          M[ 5] =  f; M[ 6] =  0; M[ 7] =  0;
	M[ 8] = 0;          M[ 9] =  0; M[10] =  A; M[11] =  B;
	M[12] = 0;          M[13] =  0; M[14] = -1; M[15] =  0;
}

// построение матрицы переноса
static void Matrix4Translation(Matrix4 M, float x, float y, float z)
{
	M[ 0] = 1; M[ 1] = 0; M[ 2] = 0; M[ 3] = x;
	M[ 4] = 0; M[ 5] = 1; M[ 6] = 0; M[ 7] = y;
	M[ 8] = 0; M[ 9] = 0; M[10] = 1; M[11] = z;
	M[12] = 0; M[13] = 0; M[14] = 0; M[15] = 1;
}

// построение матрицы вращения
static void Matrix4Rotation(Matrix4 M, float x, float y, float z)
{
	const float A = cosf(x), B = sinf(x), C = cosf(y),
	            D = sinf(y), E = cosf(z), F = sinf(z);
	const float AD = A * D, BD = B * D;

	M[ 0] = C * E;           M[ 1] = -C * F;          M[ 2] = D;      M[ 3] = 0;
	M[ 4] = BD * E + A * F;  M[ 5] = -BD * F + A * E; M[ 6] = -B * C; M[ 7] = 0;
	M[ 8] = -AD * E + B * F; M[ 9] = AD * F + B * E;  M[10] = A * C;  M[11] = 0;
	M[12] = 0;               M[13] = 0;               M[14] = 0;      M[15] = 1;
}

// перемножение двух матриц
static void Matrix4Mul(Matrix4 M, Matrix4 A, Matrix4 B)
{
	M[ 0] = A[ 0] * B[ 0] + A[ 1] * B[ 4] + A[ 2] * B[ 8] + A[ 3] * B[12];
	M[ 1] = A[ 0] * B[ 1] + A[ 1] * B[ 5] + A[ 2] * B[ 9] + A[ 3] * B[13];
	M[ 2] = A[ 0] * B[ 2] + A[ 1] * B[ 6] + A[ 2] * B[10] + A[ 3] * B[14];
	M[ 3] = A[ 0] * B[ 3] + A[ 1] * B[ 7] + A[ 2] * B[11] + A[ 3] * B[15];
	M[ 4] = A[ 4] * B[ 0] + A[ 5] * B[ 4] + A[ 6] * B[ 8] + A[ 7] * B[12];
	M[ 5] = A[ 4] * B[ 1] + A[ 5] * B[ 5] + A[ 6] * B[ 9] + A[ 7] * B[13];
	M[ 6] = A[ 4] * B[ 2] + A[ 5] * B[ 6] + A[ 6] * B[10] + A[ 7] * B[14];
	M[ 7] = A[ 4] * B[ 3] + A[ 5] * B[ 7] + A[ 6] * B[11] + A[ 7] * B[15];
	M[ 8] = A[ 8] * B[ 0] + A[ 9] * B[ 4] + A[10] * B[ 8] + A[11] * B[12];
	M[ 9] = A[ 8] * B[ 1] + A[ 9] * B[ 5] + A[10] * B[ 9] + A[11] * B[13];
	M[10] = A[ 8] * B[ 2] + A[ 9] * B[ 6] + A[10] * B[10] + A[11] * B[14];
	M[11] = A[ 8] * B[ 3] + A[ 9] * B[ 7] + A[10] * B[11] + A[11] * B[15];
	M[12] = A[12] * B[ 0] + A[13] * B[ 4] + A[14] * B[ 8] + A[15] * B[12];
	M[13] = A[12] * B[ 1] + A[13] * B[ 5] + A[14] * B[ 9] + A[15] * B[13];
	M[14] = A[12] * B[ 2] + A[13] * B[ 6] + A[14] * B[10] + A[15] * B[14];
	M[15] = A[12] * B[ 3] + A[13] * B[ 7] + A[14] * B[11] + A[15] * B[15];
}

// инициализаця OpenGL
bool GLWindowInit(const GLWindow *window)
{
	ASSERT(window);

	// устанавливаем вьюпорт на все окно
	glViewport(0, 0, window->width, window->height);

	// параметры OpenGL
	glClearColor(255.0f/255.0f, 245.0f / 255.0f, 213.0f / 255.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// создадим и загрузим шейдерные программы
	shaderProgram_colors = OpenShaderProgram("color_poly");
	if (shaderProgram_colors<=0)
		return false;

	shaderProgram_one_color = OpenShaderProgram("one_color_poly");
	if (shaderProgram_one_color <= 0)
		return false;

	// создадим перспективную матрицу
	const float aspectRatio = (float)window->width / (float)window->height;
	Matrix4Perspective(projectionMatrix, 45.0f, aspectRatio, 1.0f, 10.0f);

	// с помощью видовой матрицы отодвинем сцену назад
	Matrix4Translation(viewMatrix, 0.0f, 0.0f, -4.0f);

	Matrix4Mul(viewProjectionMatrix, projectionMatrix, viewMatrix);

	// запросим у OpenGL свободный индекс VAO
	glGenVertexArrays(1, &vertVAO);
	// сделаем VAO активным
	glBindVertexArray(vertVAO);

	// создадим VBO для данных вершин
	glGenBuffers(1, &vertVBO);
	// начинаем работу с буфером для вершин
	glBindBuffer(GL_ARRAY_BUFFER, vertVBO);
	// поместим в буфер координаты вершин куба
	glBufferData(GL_ARRAY_BUFFER, icd().multi_colors_buffer_bytes_count(), icd().multi_colors_buffer(), GL_STATIC_DRAW);

	// сделаем шейдерную программу активной
	ShaderProgramBind(shaderProgram_colors);
	// получим индекс матрицы из шейдерной программы
	modelViewProjectionMatrixLocation_colors = glGetUniformLocation(shaderProgram_colors, "modelViewProjectionMatrix");

	char* offset = 0;
	// получим индекс вершинного атрибута 'position' из шейдерной программы
	positionLocation_colors = glGetAttribLocation(shaderProgram_colors, "position");
	if (positionLocation_colors != -1)
	{
		// укажем параметры вершинного атрибута для текущего активного VBO
		glVertexAttribPointer(positionLocation_colors, 3, GL_FLOAT, GL_FALSE, GLsizei(icd().multi_colors_buffer_coord_byte_size()), offset);
		// разрешим использование вершинного атрибута
		glEnableVertexAttribArray(positionLocation_colors);
	}

	offset += 3 * sizeof(GLfloat);
	// получим индекс вершинного атрибута 'color' из шейдерной программы
	colorLocation = glGetAttribLocation(shaderProgram_colors, "color");
	if (colorLocation != -1)
	{
		// укажем параметры вершинного атрибута для текущего активного VBO
		glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, GLsizei(icd().multi_colors_buffer_coord_byte_size()), offset);
		// разрешим использование вершинного атрибута
		glEnableVertexAttribArray(colorLocation);
	}

	ShaderProgramBind(shaderProgram_one_color);

	// запросим у OpenGL свободный индекс VAO
	glGenVertexArrays(1, &vertVAO_one_color);
	// сделаем VAO активным
	glBindVertexArray(vertVAO_one_color);
	// создадим VBO для данных вершин
	glGenBuffers(1, &vertVBO_one_color);
	// начинаем работу с буфером для вершин
	glBindBuffer(GL_ARRAY_BUFFER, vertVBO_one_color);
	// поместим в буфер координаты вершин куба
	glBufferData(GL_ARRAY_BUFFER, icd().one_color_buffer_bytes_count(), icd().one_color_buffer(), GL_STATIC_DRAW);

	// получим индекс матрицы из шейдерной программы
	modelViewProjectionMatrixLocation_one_color = glGetUniformLocation(shaderProgram_one_color, "modelViewProjectionMatrix");

	sketchColorLocation = glGetUniformLocation(shaderProgram_one_color, "sketchColor");
	if (sketchColorLocation != -1)
		glUniform4fv(sketchColorLocation, 1, icd().sketch_color());

	// получим индекс вершинного атрибута 'position' из шейдерной программы
	positionLocation_one_color = glGetAttribLocation(shaderProgram_one_color, "position");
	if (positionLocation_one_color != -1)
	{
		// укажем параметры вершинного атрибута для текущего активного VBO
		glVertexAttribPointer(positionLocation_one_color, 3, GL_FLOAT, GL_FALSE, GLsizei(icd().one_color_buffer_coord_byte_size()), 0);
		// разрешим использование вершинного атрибута
		glEnableVertexAttribArray(positionLocation_one_color);
	}

	//glLineWidth(2.0);

	// проверим не было ли ошибок
	OPENGL_CHECK_FOR_ERRORS();

	return true;
}

// очистка OpenGL
void GLWindowClear(const GLWindow *window)
{
	ASSERT(window);

	// делаем текущие VBO неактивными
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// удаляем VBO
	glDeleteBuffers(1, &vertVBO);

	// далаем текущий VAO неактивным
	glBindVertexArray(0);
	// удаляем VAO
	glDeleteVertexArrays(1, &vertVAO);

	// удаляем шейдерную программу
	ShaderProgramDestroy(shaderProgram_colors);
}

// функция рендера
void GLWindowRender(const GLWindow *window)
{
	ASSERT(window);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// рисовка цветов треугольничков
	ShaderProgramBind(shaderProgram_colors);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// передаем в шейдер матрицу преобразования координат вершин
	if (modelViewProjectionMatrixLocation_colors != -1)
		glUniformMatrix4fv(modelViewProjectionMatrixLocation_colors, 1, GL_TRUE, modelViewProjectionMatrix);

	// выводим на экран все что относится к VAO
	glBindVertexArray(vertVAO);
	glDrawArrays(GL_TRIANGLES, 0, GLsizei(icd().multi_colors_buffer_coords_count()));

#if 0
 	// рисовка каркаса
	ShaderProgramBind(shaderProgram_one_color);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (modelViewProjectionMatrixLocation_one_color != -1)
		glUniformMatrix4fv(modelViewProjectionMatrixLocation_one_color, 1, GL_TRUE, modelViewProjectionMatrix);

	glBindVertexArray(vertVAO_one_color);
	glDrawArrays(GL_TRIANGLES, 0, GLsizei(icd().one_color_buffer_coords_count()));
#endif

	// проверка на ошибки
	OPENGL_CHECK_FOR_ERRORS();
}

void rotation_angle_change(float* cubeRotation, int scr_ax_num, bool increase, double deltaTime)
{
	float d = 5.0f * (float)deltaTime;
	float n = cubeRotation[scr_ax_num] + (float)(increase ? d : -d);
	if (n > 360.0f)
		n -= 360.0f;
	if (n < -360.0f)
		n += 360.0f;
	cubeRotation[scr_ax_num] = n;
}

// функция обновления
void GLWindowUpdate(const GLWindow *window, double deltaTime)
{
	if (!icd().rotation_animation())
		return;

	ASSERT(window);
	ASSERT(deltaTime >= 0.0); // проверка на возможность бага

	rotation_angle_change(cubeRotation, icd().rotate_animation_screen_axis(), icd().rotation_animation_angle_increase(), deltaTime);

	// рассчитаем матрицу преобразования координат вершин куба	
	Matrix4 modelMatrix; // матрица вращения кубика
	Matrix4Rotation(modelMatrix, cubeRotation[0], cubeRotation[1], cubeRotation[2]);
	Matrix4Mul(modelViewProjectionMatrix, viewProjectionMatrix, modelMatrix);
}

// функция обработки ввода с клавиатуры и мыши
void GLWindowInput(const GLWindow *window)
{
	ASSERT(window);

	// выход из приложения по кнопке Esc
	if (InputIsKeyPressed(VK_ESCAPE))
		GLWindowDestroy();

	if (InputIsKeyPressed(VK_SPACE))
		icd().set_rotation_animation(!icd().rotation_animation());

	if (InputIsKeyPressed(VK_RIGHT))
	{
		icd().set_rotate_animation_screen_axis(1, false);
		icd().set_rotation_animation(true);
	}
	if (InputIsKeyPressed(VK_LEFT))
	{
		icd().set_rotate_animation_screen_axis(1, true);
		icd().set_rotation_animation(true);
	}

	// переключение между оконным и полноэкранным режимом
	// осуществляется по нажатию комбинации Alt+Enter
	if (InputIsKeyDown(VK_MENU) && InputIsKeyPressed(VK_RETURN))
		GLWindowSetSize(window->width, window->height, !window->fullScreen);
}

int ic_draw(const IcosamateInSpace& ic)
{
	int result = -1;

	if (!GLWindowCreate("icosomate draw", 1200, 800, false))
		return 1;

	icd().set_icosomate(ic);

	result = GLWindowMainLoop();
	GLWindowDestroy();

	return result;
}
