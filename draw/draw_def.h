#pragma once

#include "OpenGL.h"
#include "matrix.h"
#include "../coord.h"

struct OGLObjs
{
	GLuint vbo_ = -1;
	GLuint vao_ = -1;
	GLuint program_ = 0;

	GLint model_view_projection_matrix_location_ = -1;
	GLint position_location_ = -1;
	GLint color_location_ = -1;

	bool inited() const { return vao_ != -1; }

	~OGLObjs();
};


void clear(OGLObjs& glo);
void set(OGLObjs& glo, const glm::mat4& model_view_projection_matrix);

// буфер, где каждый элемент состоит из подряд идущих N примитивов типа F
template<class F, size_t N> struct DBuffer
{
	std::vector<F> buf_;
	bool empty() const { return buf_.empty(); }
	const F* ptr() const { return &buf_[0]; }
	size_t prim_byte_size() const { return sizeof(F); }
	size_t elem_size() const { return N; }
	size_t elem_byte_size() const { return N * sizeof(F); }
	size_t elems_count() const { return buf_.size() / N; }
	size_t bytes_count() const { return buf_.size() * sizeof(F); }
};

template<class B> void complete(const OGLObjs& glo, const B& buf)
{
	if (glo.inited())
	{
		glUseProgram(glo.program_);
		glBindVertexArray(glo.vao_);
		glBindBuffer(GL_ARRAY_BUFFER, glo.vbo_);
		glBufferData(GL_ARRAY_BUFFER, buf.bytes_count(), buf.ptr(), GL_STATIC_DRAW);
	}
}

bool open_program(GLuint& pr, const char* sh_fname);
void prepare_one_color_drawing(OGLObjs& glo, const float* buf, size_t buffer_bytes_count, size_t one_elem_byte_size, const float* col);

void add(std::vector<float>& buf, const Coord& c);
void add(std::vector<float>& buf, const Coord& c0, const Coord& c1, const Coord& c2);
void add(std::vector<float>& buf, const CoordS& cs);
void assign(std::vector<float>& buf, size_t i, const Coord& c);





