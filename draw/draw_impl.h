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

// количество вершин
static const uint32_t verticesCount = 9;
const int vertexSize = 7 * sizeof(float);

// буфер вершин
static const float s = 1.0f; // половина размера куба
static const float vertBuffer[verticesCount][7] = {
	{-s, s, s, 1.0f, 0.0f, 0.0f, 1.0f}, { s, s, s, 1.0f, 0.0f, 0.0f, 1.0f}, { s,-s, s, 1.0f, 0.0f, 0.0f, 1.0f},
	{-s, s, s, 1.0f, 0.0f, 0.0f, 1.0f}, { s,-s, s, 1.0f, 0.0f, 0.0f, 1.0f}, { -s, -s, s, 1.0f, 0.0f, 0.0f, 1.0f},
	{-s, s,-s, 0.0f, 1.0f, 0.0f, 1.0f}, {-s, s, s, 0.0f, 1.0f, 0.0f, 1.0f}, {-s,-s, s, 0.0f, 1.0f, 0.0f, 1.0f}
};

class IcosamateDrawing
{
	std::vector<float> one_color_buffer_;
	void add_to_one_color_buffer(const Coord& c);
	void fill_one_color_buffer_faces(); // треугольники граней икосаэдра
	void fill_one_color_buffer_faces_subtriangles(); // каждая грань икосаэдра делится на четыре элемента-треугольничка
	void fill_one_color_buffer();

	float sketch_color_[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
public:
	IcosamateDrawing();

	const float* one_color_buffer() const { return &one_color_buffer_[0]; }
	size_t one_color_buffer_coords_count() const { return one_color_buffer_.size() / 3; }
	size_t one_color_buffer_bytes_count() const { return one_color_buffer_.size() * sizeof(float); }
	size_t one_color_buffer_coord_byte_size() const { return 3 * sizeof(float); }
	const float* sketch_color() const { return sketch_color_; }
};

IcosamateDrawing& icd();

