#pragma once

#include <vector>
#include "OpenGL.h"
#include "../coord.h"

// ������ �������
typedef float Matrix4[16];

// ��������� ��� �������� ��������������� ��������� ��������� � ��������
static GLuint shaderProgram_colors = 0, shaderProgram_one_color = 0, colorTexture = 0;

// ��� �������� ���� ����� �������� ����
static float  cubeRotation[3] = { 0.0f, 0.0f, 0.0f };

// ������� ��������������
static Matrix4 modelViewProjectionMatrix = { 0.0f }, viewMatrix = { 0.0f },
projectionMatrix = { 0.0f }, viewProjectionMatrix = { 0.0f };

// ������� ���������� �� ��������� ���������
static GLint modelViewProjectionMatrixLocation_colors = -1, positionLocation_colors = -1, colorLocation = -1,
modelViewProjectionMatrixLocation_one_color = -1, positionLocation_one_color = -1, sketchColorLocation = -1;

// ��� �������� VAO � VBO ��������� ���������
static GLuint vertVBO = -1;
static GLuint vertVBO_one_color = -1;
static GLuint vertVAO = -1;
static GLuint vertVAO_one_color = -1;

typedef int DrawColor;

class IcosamateDrawing
{
	IcosamateInSpace ic_;

	std::vector<float> one_color_buffer_; // ���������� ������ �������������, �������������� sketch_color_
	void add_to_one_color_buffer(const Coord& c);
	void add_to_one_color_buffer(const Coord& c0, const Coord& c1, const Coord& c2);
	void fill_one_color_buffer_faces(); // ������������ ������ ���������
	void fill_one_color_buffer_faces_subtriangles(); // ������ ����� ��������� ������� �� ������ ��������-�������������
	void fill_one_color_buffer_not_stickered(); // �� ������ ��������-������������� �������� ��������� ������ �������
	void fill_one_color_buffer();
	float sketch_color_[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	std::vector<float> multi_colors_buffer_;  // ���������� ������	������������� ������ � ������, ����� ������ �������
	void fill_multi_colors_buffer(bool colors_only=false);

	std::vector<DrawColor> draw_colors_;
	std::vector<float> gl_face_colors_;
	void fill_gl_face_colors();

	bool rotation_animation_ = true;
	int rotate_animation_screen_axis_ = 1;
	bool rotation_animation_angle_increase_ = true;

public:
	IcosamateDrawing();

	void set_icosomate(const IcosamateInSpace& ic) 
	{
		ic_ = ic; 
		fill_multi_colors_buffer(true);
	}
	bool rotation_animation() const { return rotation_animation_; }
	void set_rotation_animation(bool r) { rotation_animation_ = r; }
	int rotate_animation_screen_axis() const { return rotate_animation_screen_axis_; }
	bool rotation_animation_angle_increase() const { return rotation_animation_angle_increase_; }
	void set_rotate_animation_screen_axis(int r, bool i) { rotate_animation_screen_axis_ = r; rotation_animation_angle_increase_ = i; }

	const float* one_color_buffer() const { return &one_color_buffer_[0]; }
	size_t one_color_buffer_coords_count() const { return one_color_buffer_.size() / 3; }
	size_t one_color_buffer_bytes_count() const { return one_color_buffer_.size() * sizeof(float); }
	size_t one_color_buffer_coord_byte_size() const { return 3 * sizeof(float); }
	const float* sketch_color() const { return sketch_color_; }

	const float* multi_colors_buffer() const { return &multi_colors_buffer_[0]; }
	size_t multi_colors_buffer_coords_count() const { return multi_colors_buffer_.size() / 7; }
	size_t multi_colors_buffer_bytes_count() const { return multi_colors_buffer_.size() * sizeof(float); }
	size_t multi_colors_buffer_coord_byte_size() const { return 7 * sizeof(float); }
};

IcosamateDrawing& icd();

