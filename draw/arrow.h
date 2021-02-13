#pragma once

#include "draw_def.h"
#include "../coord.h"
#include "matrix.h"

enum class ArrowsType
{
	VertElems,
	VertElemsRotations,
	Centers
};

class Arrows
{
	double arrow_end_ = 0.2;
	float color_[4];

	OGLObjs glo_;
	DBuffer<float, 3> buffer_;

	std::vector<CoordS> coords_;

	bool visible_ = true;

public:
	void set_arrow_end(double ae) { arrow_end_ = ae; }
	void set_color(const float* color);
	bool empty() const { return coords_.empty(); }
	void clear_coords();
	void clear();
	void add_arrow(const CoordS& coords);
	void complete();
	void gl_init();
	bool visible() const { return visible_; }
	void set_visible(bool v) { visible_=v; }
	void render(const glm::mat4& pvm);
};

