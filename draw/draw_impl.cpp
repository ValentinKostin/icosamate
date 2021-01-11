#include "draw_impl.h"
#include "../coord.h"

IcosomateCoords gic;

IcosamateDrawing& icd()
{
	static IcosamateDrawing icd;
	return icd;
}

void IcosamateDrawing::add_to_one_color_buffer(const Coord& c)
{
	one_color_buffer_.push_back(float(c.x_));
	one_color_buffer_.push_back(float(c.y_));
	one_color_buffer_.push_back(float(c.z_));
}

void IcosamateDrawing::fill_one_color_buffer()
{
	size_t n = IcosamateInSpace::FACE_COUNT;
	one_color_buffer_.reserve(3*n);
	for (FaceTriangleId id = 0; id < n; id++)
	{
		const FaceTriangle& t = gic.face_triangle(id);
		for (size_t i = 0; i < 3; ++i)
		{
			add_to_one_color_buffer(*t.coords_[0]);
			add_to_one_color_buffer(*t.coords_[1]);
			add_to_one_color_buffer(*t.coords_[2]);
		}
	}
}

IcosamateDrawing::IcosamateDrawing()
{
	fill_one_color_buffer();
}
