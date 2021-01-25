#pragma once

#include "draw_def.h"
#include "../coord.h"
#include "matrix.h"

enum class ArrowsType
{
	VertElems
};

class Arrows
{
	float color_[4];

	OGLObjs glo_;
	DBuffer<float, 3> buffer_;

	std::vector<CoordS> coords_;

	bool visible_ = true;

public:
	Arrows(const float* color);
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

