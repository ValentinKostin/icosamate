#include "arrow.h"

CoordS perp_circle(const Coord& center, const Coord& dir, double r)
{
	Coord ex{ 1,0,0 }, ey{ 0,1,0 };
	Coord e = vect_prod(ex, dir).norm_sq() > vect_prod(ey, dir).norm_sq() ? ex : ey;
	Coord e1 = vect_prod(e, dir);
	e1.normalize();
	Coord e2 = vect_prod(e1, dir);
	e2.normalize();
	CoordS dirs = {e1, (e1+e2).normalize(), e2, (e2 - e1).normalize(), -e1, -(e1 + e2).normalize(), -e2, (e1 - e2).normalize() };
	CoordS ret;
	ret.reserve(dirs.size());
	for (const auto& d : dirs)
		ret.push_back(center + d*r);
	return ret;
}

void add_triangle(CoordS& cs, const Coord& c1, const Coord& c2, const Coord& c3)
{
	cs.push_back(c1);
	cs.push_back(c2);
	cs.push_back(c3);
}

CoordS tube_coords(const Coord& c1, const Coord& c2, double r1, double r2)
{
	Coord dir = c2 - c1;
	CoordS cir1 = perp_circle(c1, dir, r1);
	CoordS cir2 = perp_circle(c2, dir, r2);
	check(cir1.size() == cir2.size());
	size_t n = cir1.size();

	CoordS r;
	for (size_t i=0; i<cir1.size(); ++i)
	{
		add_triangle(r, cir1[i], cir2[i], cir2[(i + 1) % n]);
		add_triangle(r, cir1[i], cir2[(i + 1) % n], cir1[(i + 1) % n]);
	}
	return r;
}

void Arrows::set_color(const float* color)
{
	memcpy(color_, color, sizeof(float) * 4);
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
	const double tube_radius = 0.0075;

	CoordS tub_cs;
	check(coords.size() >= 2);
	for (size_t i = 0; i+1 < coords.size(); i++)
	{
		CoordS tmp_cs = tube_coords(coords[i], coords[i + 1], tube_radius, tube_radius);
		tub_cs.insert(tub_cs.end(), tmp_cs.begin(), tmp_cs.end());
	}

	coords_.push_back(tub_cs);
	add(buffer_.buf_, tub_cs);
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

	GLint index = 0;
	for (size_t i = 0; i < coords_.size(); i++)
	{
		glDrawArrays(GL_TRIANGLES, index, GLsizei(coords_[i].size()));
		index += (GLint)coords_[i].size();
	}
}
