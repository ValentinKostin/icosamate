#include "arrow.h"

Arrows::Arrows(const float* color)
{
	memcpy(color_, color, sizeof(float) * 4);
	coords_.reserve(10);
}

void Arrows::clear_coords()
{
	coords_.clear();
	buffer_.buf_.clear();
}

void Arrows::clear()
{
	clear_coords();
	::clear(glo_);
}

void Arrows::add_arrow(const CoordS& coords)
{
	coords_.push_back(coords);
	add(buffer_.buf_, coords);
}

void Arrows::complete()
{
	if (!empty() && glo_.inited())
	{
		glUseProgram(glo_.program_);
		glBindVertexArray(glo_.vao_);
		glBindBuffer(GL_ARRAY_BUFFER, glo_.vbo_);
		glBufferData(GL_ARRAY_BUFFER, buffer_.bytes_count(), buffer_.ptr(), GL_STATIC_DRAW);
	}
}

void Arrows::gl_init()
{
	prepare_one_color_drawing(glo_, empty() ? nullptr : buffer_.ptr(), buffer_.bytes_count(), buffer_.elem_byte_size(), color_);
}

void Arrows::render(const glm::mat4& pvm)
{
	if (empty())
		return;
	if (!visible())
		return;

	glUseProgram(glo_.program_);
	set(glo_, pvm);
	glBindVertexArray(glo_.vao_);

	size_t index = 0;
	for (size_t i = 0; i < coords_.size(); i++)
	{
		glDrawArrays(GL_LINE_STRIP, 0, GLsizei(coords_[i].size()));
		index += coords_[i].size();
	}
}
