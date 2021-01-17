#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "common.h"
#include "GLWindow.h"
#include "draw.h"
#include "draw_impl.h"

// построение перспективной матрицы
void Matrix4Perspective(Matrix4 M, float fovy, float aspect, float znear, float zfar)
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
void Matrix4Translation(Matrix4 M, float x, float y, float z)
{
	M[ 0] = 1; M[ 1] = 0; M[ 2] = 0; M[ 3] = x;
	M[ 4] = 0; M[ 5] = 1; M[ 6] = 0; M[ 7] = y;
	M[ 8] = 0; M[ 9] = 0; M[10] = 1; M[11] = z;
	M[12] = 0; M[13] = 0; M[14] = 0; M[15] = 1;
}

// построение матрицы вращения
void Matrix4Rotation(Matrix4 M, float x, float y, float z)
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
void Matrix4Mul(Matrix4 M, Matrix4 A, Matrix4 B)
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

	return icd().opengl_init(window->width, window->height);
}

// очистка OpenGL
void GLWindowClear(const GLWindow *window)
{
	ASSERT(window);

	icd().opengl_clear();
}

// функция рендера
void GLWindowRender(const GLWindow *window)
{
	ASSERT(window);

	icd().render();
}

// функция обновления
void GLWindowUpdate(const GLWindow *window, double deltaTime)
{
	ASSERT(window);

	icd().update();
}

// функция обработки ввода с клавиатуры и мыши
void GLWindowInput(const GLWindow *window)
{
	ASSERT(window);

	// выход из приложения по кнопке Esc
	if (InputIsKeyPressed(VK_ESCAPE))
		GLWindowDestroy();

	if (InputIsKeyPressed(VK_F2))
		icd().set_mode(icd().mode() == DrawMode::DARK ? DrawMode::LIGHT : DrawMode::DARK);

	if (InputIsKeyPressed(VK_SPACE))
		icd().set_rotation_animation(!icd().rotation_animation());

	if (InputIsKeyPressed(VK_RIGHT))
	{
		icd().set_rotate_animation_screen_axis(1, true);
		icd().set_rotation_animation(true);
	}
	if (InputIsKeyPressed(VK_LEFT))
	{
		icd().set_rotate_animation_screen_axis(1, false);
		icd().set_rotation_animation(true);
	}
	if (InputIsKeyPressed(VK_UP))
	{
		if (InputIsKeyDown(VK_SHIFT))
			icd().set_rotate_animation_screen_axis(2, false);
		else
			icd().set_rotate_animation_screen_axis(0, false);
		icd().set_rotation_animation(true);
	}
	if (InputIsKeyPressed(VK_DOWN))
	{
		if (InputIsKeyDown(VK_SHIFT))
			icd().set_rotate_animation_screen_axis(2, true);
		else
			icd().set_rotate_animation_screen_axis(0, true);
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

	if (!GLWindowCreate("icosomate draw", 1200, 1000, false))
		return 1;

	icd().set_icosomate(ic);

	result = GLWindowMainLoop();
	GLWindowDestroy();

	return result;
}
