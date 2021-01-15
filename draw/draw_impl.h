#pragma once

#include <vector>
#include "OpenGL.h"
#include "../coord.h"

// данные матрицы
typedef float Matrix4[16];

// пременные для хранения идентификаторов шейдерной программы и текстуры
static GLuint shaderProgram_colors = 0, shaderProgram_one_color = 0, colorTexture = 0;

// для хранения двух углов поворота куба
static float  cubeRotation[3] = { 0.0f, 0.0f, 0.0f };

// матрицы преобразования
static Matrix4 modelViewProjectionMatrix = { 0.0f }, viewMatrix = { 0.0f },
projectionMatrix = { 0.0f }, viewProjectionMatrix = { 0.0f };

// индексы полученный из шейдерной программы
static GLint modelViewProjectionMatrixLocation_colors = -1, positionLocation_colors = -1, colorLocation = -1,
modelViewProjectionMatrixLocation_one_color = -1, positionLocation_one_color = -1, sketchColorLocation = -1;

// для хранения VAO и VBO связанных вершинами
static GLuint vertVBO = -1;
static GLuint vertVBO_one_color = -1;
static GLuint vertVAO = -1;
static GLuint vertVAO_one_color = -1;

typedef int DrawColor;

class IcosamateDrawing
{
	IcosamateInSpace ic_;

	std::vector<float> one_color_buffer_; // координаты вершин треугольников, отрисовываемые sketch_color_
	void add_to_one_color_buffer(const Coord& c);
	void fill_one_color_buffer_faces(); // треугольники граней икосаэдра
	void fill_one_color_buffer_faces_subtriangles(); // каждая грань икосаэдра делится на четыре элемента-треугольничка
	void fill_one_color_buffer();
	float sketch_color_[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	std::vector<float> multi_colors_buffer_;  // координаты вершин	треугольников вместе с цветом, после каждой вершины
	void fill_multi_colors_buffer(bool colors_only=false);

	std::vector<DrawColor> draw_colors_;
	std::vector<float> gl_face_colors_;
	void fill_gl_face_colors();

public:
	IcosamateDrawing();

	void set_icosomate(const IcosamateInSpace& ic) 
	{
		ic_ = ic; 
		fill_multi_colors_buffer(true);
	}

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

