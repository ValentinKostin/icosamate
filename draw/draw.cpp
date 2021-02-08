#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "common.h"
#include "GLWindow.h"
#include "../action.h"
#include "draw.h"
#include "draw_impl.h"
#include "../library.h"

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
		icd().set_autorotation(!icd().autorotation());

	if (InputIsKeyPressed('Q'))
		icd().set_draw_axes(!icd().draw_axes());
	if (InputIsKeyPressed('W'))
		icd().set_arrows_visible(ArrowsType::VertElems, !icd().is_arrows_visible(ArrowsType::VertElems));
	if (InputIsKeyPressed('R'))
		icd().inverse();

	bool clockwise = !InputIsKeyDown(VK_SHIFT);
	bool ctrl = InputIsKeyDown(VK_CONTROL);
	typedef void(IcosamateDrawing::* ChangeFun)(char, bool);
	ChangeFun change_fun = ctrl ? &IcosamateDrawing::move : &IcosamateDrawing::turn;
	for (char c = 'A'; c <= 'L'; ++c)
	{
		if (InputIsKeyPressed(c))
			(icd().*change_fun)(c, clockwise);
	}
	if (ctrl && InputIsKeyPressed('Z'))
		icd().undo();
	if (ctrl && InputIsKeyPressed('S'))
		library().save(icd());

	icd().set_rotation_animation(true);

	if (InputIsKeyDown(VK_RIGHT))
	{
		if (InputIsKeyDown(VK_SHIFT))
			icd().set_rotate_animation_screen_axis(2, false);
		else
			icd().set_rotate_animation_screen_axis(1, true);
		icd().set_autorotation(false);
	}
	else if (InputIsKeyDown(VK_LEFT))
	{
		if (InputIsKeyDown(VK_SHIFT))
			icd().set_rotate_animation_screen_axis(2, true);
		else
			icd().set_rotate_animation_screen_axis(1, false);
		icd().set_autorotation(false);
	}
	else if (InputIsKeyDown(VK_UP))
	{
		if (InputIsKeyDown(VK_SHIFT))
			icd().set_rotate_animation_screen_axis(2, false);
		else
			icd().set_rotate_animation_screen_axis(0, false);
		icd().set_autorotation(false);
	}
	else if (InputIsKeyDown(VK_DOWN))
	{
		if (InputIsKeyDown(VK_SHIFT))
			icd().set_rotate_animation_screen_axis(2, true);
		else
			icd().set_rotate_animation_screen_axis(0, true);
		icd().set_autorotation(false);
	}
	else if (!icd().autorotation())
		icd().set_rotation_animation(false);

	// переключение между оконным и полноэкранным режимом
	// осуществляется по нажатию комбинации Alt+Enter
	if (InputIsKeyDown(VK_MENU) && InputIsKeyPressed(VK_RETURN))
		GLWindowSetSize(window->width, window->height, !window->fullScreen);
}

int ic_draw(const IcosamateInSpace& ic, const std::string& turnig_algorithm)
{
	int result = -1;

	if (!GLWindowCreate("icosomate", 1200, 1200, false))
		return 1;

	icd().set_icosomate(ic);
	icd().set_turnig_algorithm(turnig_algorithm);

	result = GLWindowMainLoop();
	GLWindowDestroy();

	return result;
}

int ic_scramble(const std::string& turnig_algorithm)
{
	IcosamateInSpace ic;
	ActionS acts = IcosamateInSpace::from_str(turnig_algorithm);
	ic.actions(acts);

	return ic_draw(ic, turnig_algorithm);
}
