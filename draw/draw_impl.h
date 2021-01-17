#pragma once

#include <vector>
#include "OpenGL.h"
#include "../coord.h"

// ������ �������
typedef float Matrix4[16];

void Matrix4Perspective(Matrix4 M, float fovy, float aspect, float znear, float zfar);
void Matrix4Translation(Matrix4 M, float x, float y, float z);
void Matrix4Rotation(Matrix4 M, float x, float y, float z);
void Matrix4Mul(Matrix4 M, Matrix4 A, Matrix4 B);


typedef int DrawColor;

enum class DrawMode { DARK = 0, LIGHT = 1 };

class IcosamateDrawing
{
	// ��������� ��� �������� ��������������� ��������� ��������� � ��������
	GLuint shaderProgram_colors = 0, shaderProgram_one_color = 0;

	// ������� ��������������
	Matrix4 model_matrix_ = { 0.0f }, modelViewProjectionMatrix = { 0.0f }, viewMatrix = { 0.0f },
		projectionMatrix = { 0.0f }, viewProjectionMatrix = { 0.0f };

	// ������� ���������� �� ��������� ���������
	GLint modelViewProjectionMatrixLocation_colors = -1, positionLocation_colors = -1, colorLocation = -1,
		modelViewProjectionMatrixLocation_one_color = -1, positionLocation_one_color = -1, sketchColorLocation = -1;

	// ��� �������� VAO � VBO ��������� ���������
	GLuint vertVBO = -1;
	GLuint vertVBO_one_color = -1;
	GLuint vertVAO = -1;
	GLuint vertVAO_one_color = -1;

	float clear_color_[4] = { 255.0f / 255.0f, 245.0f / 255.0f, 213.0f / 255.0f, 1.0f };

	IcosamateInSpace ic_;

	std::vector<float> one_color_buffer_; // ���������� ������ �������������, �������������� sketch_color_
	void fill_one_color_buffer_faces(); // ������������ ������ ���������
	void fill_one_color_buffer_faces_subtriangles(); // ������ ����� ��������� ������� �� ������ ��������-�������������
	void fill_one_color_buffer_not_stickered(); // �� ������ ��������-������������� �������� ��������� ������ �������
	void fill_one_color_buffer();
	float sketch_color_[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const float* one_color_buffer() const { return &one_color_buffer_[0]; }
	size_t one_color_buffer_coords_count() const { return one_color_buffer_.size() / 3; }
	size_t one_color_buffer_bytes_count() const { return one_color_buffer_.size() * sizeof(float); }
	size_t one_color_buffer_coord_byte_size() const { return 3 * sizeof(float); }

	std::vector<float> multi_colors_buffer_;  // ���������� ������	������������� ������ � ������, ����� ������ �������
	void fill_multi_colors_buffer(bool colors_only=false);
	const float* multi_colors_buffer() const { return &multi_colors_buffer_[0]; }
	size_t multi_colors_buffer_coords_count() const { return multi_colors_buffer_.size() / 7; }
	size_t multi_colors_buffer_bytes_count() const { return multi_colors_buffer_.size() * sizeof(float); }
	size_t multi_colors_buffer_coord_byte_size() const { return 7 * sizeof(float); }

	std::vector<DrawColor> draw_colors_;
	std::vector<float> gl_face_colors_;
	void fill_gl_face_colors();

	bool autorotation_ = false;
	bool rotation_animation_ = false;
	int rotate_animation_screen_axis_ = 1;
	bool rotation_animation_angle_increase_ = true;

	DrawMode draw_mode_ = DrawMode::DARK;

	size_t start_tick_count_ = 0;

public:
	IcosamateDrawing();
	bool opengl_init(int w_width, int w_height);
	void opengl_clear();
	void render();
	void update();

	const float* clear_color() const { return draw_mode_== DrawMode::LIGHT ? clear_color_ : sketch_color_; }
	const float* sketch_color() const { return draw_mode_ == DrawMode::LIGHT ? sketch_color_ : clear_color_; }
	
	void set_mode(DrawMode ddm);
	DrawMode mode() const { return draw_mode_; }	

	void set_icosomate(const IcosamateInSpace& ic) 
	{
		ic_ = ic; 
		fill_multi_colors_buffer(true);
	}
	bool autorotation() const { return autorotation_; }
	void set_autorotation(bool r) { autorotation_ = r; }
	bool rotation_animation() const { return rotation_animation_; }
	void set_rotation_animation(bool r);
	int rotate_animation_screen_axis() const { return rotate_animation_screen_axis_; }
	bool rotation_animation_angle_increase() const { return rotation_animation_angle_increase_; }
	void set_rotate_animation_screen_axis(int r, bool i) { rotate_animation_screen_axis_ = r; rotation_animation_angle_increase_ = i; }
};

IcosamateDrawing& icd();

