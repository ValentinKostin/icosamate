#pragma once

#include <vector>
#include "OpenGL.h"
#include "../coord.h"
#include "draw_def.h"
#include "matrix.h"
#include "arrow.h"

typedef int DrawColor;

enum class DrawMode { DARK = 0, LIGHT = 1 };

typedef Coord GLPix;

class IcosamateDrawing
{
	// матрицы преобразования
	glm::mat4 model_matrix_, model_view_projection_matrix_, view_matrix_, projection_matrix_, view_projection_matrix_;

	OGLObjs glo_vert_one_color_, glo_multi_colors_, glo_axis_;

	float clear_color_[4] = { 255.0f / 255.0f, 245.0f / 255.0f, 213.0f / 255.0f, 1.0f };

	IcosamateInSpace ic_;
	const IcosamateInSpace ic0_;

	DBuffer<float, 3> one_color_buffer_; // координаты вершин треугольников, отрисовываемые sketch_color_
	void fill_one_color_buffer_faces(); // треугольники граней икосаэдра
	void fill_one_color_buffer_faces_subtriangles(); // каждая грань икосаэдра делится на четыре элемента-треугольничка
	void fill_one_color_buffer_not_stickered(); // на каждом элементе-треугольничке рисуется окантовка вокруг стикера
	void fill_one_color_buffer();
	float sketch_color_[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	DBuffer<float, 7> multi_colors_buffer_;  // координаты вершин	треугольников вместе с цветом, после каждой вершины
	void fill_multi_colors_buffer(bool colors_only = false);

	bool draw_axes_ = false;
	DBuffer<float, 3> axis_coords_buffer_;
	void fill_axis_coords_buffer();
	float axis_color_[4] = { 0.0f, 0.0f, 1.0f, 1.0f };

	Arrows ve_arrows_;
	CoordS define_arc(AxisId ax_id_1, AxisId ax_id_2);
	void fill_ve_arrows_coords();
	const float ve_arrows_color_[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

	std::vector<DrawColor> draw_colors_;
	std::vector<float> gl_face_colors_;
	void fill_gl_face_colors();

	bool autorotation_ = false;
	bool rotation_animation_ = false;
	int rotate_animation_screen_axis_ = 1;
	bool rotation_animation_angle_increase_ = true;

	DrawMode draw_mode_ = DrawMode::DARK;

	size_t start_tick_count_ = 0;

	class TextDrawing* text_drawing_ = nullptr;

	std::string turnig_algorithm_;

	void render_axis_texts();

	int w_width_ = 0, w_height_ = 0;

	// преобразование координаты -> пиксели экрана
	GLPix to_pix(const Coord& c) const;

public:
	IcosamateDrawing();
	~IcosamateDrawing();
	bool opengl_init(int w_width, int w_height);
	void opengl_clear();
	void render();
	void update();

	const float* clear_color() const { return draw_mode_ == DrawMode::LIGHT ? clear_color_ : sketch_color_; }
	const float* sketch_color() const { return draw_mode_ == DrawMode::LIGHT ? sketch_color_ : clear_color_; }
	const float* axis_color() const { return axis_color_; }

	void set_mode(DrawMode ddm);
	DrawMode mode() const { return draw_mode_; }

	void set_icosomate(const IcosamateInSpace& ic)
	{
		ic_ = ic;
		fill_multi_colors_buffer(true);
	}
	void set_turnig_algorithm(const std::string& turnig_algorithm) { turnig_algorithm_ = turnig_algorithm; }

	bool autorotation() const { return autorotation_; }
	void set_autorotation(bool r) { autorotation_ = r; }
	bool rotation_animation() const { return rotation_animation_; }
	void set_rotation_animation(bool r);
	int rotate_animation_screen_axis() const { return rotate_animation_screen_axis_; }
	bool rotation_animation_angle_increase() const { return rotation_animation_angle_increase_; }
	void set_rotate_animation_screen_axis(int r, bool i) { rotate_animation_screen_axis_ = r; rotation_animation_angle_increase_ = i; }
	bool draw_axes() const { return draw_axes_; }
	void set_draw_axes(bool r) { draw_axes_ = r; }

	void set_arrows_visible(ArrowsType at, bool visible);
	bool is_arrows_visible(ArrowsType at) const;
};

IcosamateDrawing& icd();

