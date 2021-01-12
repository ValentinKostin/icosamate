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

void IcosamateDrawing::fill_one_color_buffer_faces()
{
	size_t n = IcosamateInSpace::FACE_COUNT;
	one_color_buffer_.reserve(3 * n);
	for (FaceTriangleId id = 0; id < n; id++)
	{
		const FaceTriangle& t = gic.face_triangle(id);
		for (size_t i = 0; i < 3; ++i)
		{
			add_to_one_color_buffer(t.vert_coords(0));
			add_to_one_color_buffer(t.vert_coords(1));
			add_to_one_color_buffer(t.vert_coords(2));
		}
	}
}

void IcosamateDrawing::fill_one_color_buffer_faces_subtriangles()
{
	size_t n = IcosamateInSpace::FACE_COUNT;
	one_color_buffer_.reserve(3 * 4* n);
	for (FaceTriangleId id = 0; id < n; id++)
	{
		const FaceTriangle& t = gic.face_triangle(id);
		for (size_t i = 0; i < 3; ++i)
		{
			add_to_one_color_buffer(t.vertex_subtriangle_coord(0, 0));
			add_to_one_color_buffer(t.vertex_subtriangle_coord(0, 1));
			add_to_one_color_buffer(t.vertex_subtriangle_coord(0, 2));
			add_to_one_color_buffer(t.vertex_subtriangle_coord(1, 0));
			add_to_one_color_buffer(t.vertex_subtriangle_coord(1, 1));
			add_to_one_color_buffer(t.vertex_subtriangle_coord(1, 2));
			add_to_one_color_buffer(t.vertex_subtriangle_coord(2, 0));
			add_to_one_color_buffer(t.vertex_subtriangle_coord(2, 1));
			add_to_one_color_buffer(t.vertex_subtriangle_coord(2, 2));
			add_to_one_color_buffer(t.center_subtriangle_coord(0));
			add_to_one_color_buffer(t.center_subtriangle_coord(1));
			add_to_one_color_buffer(t.center_subtriangle_coord(2));
		}
	}
}

void IcosamateDrawing::fill_one_color_buffer()
{
	fill_one_color_buffer_faces_subtriangles();
}

IcosamateDrawing::IcosamateDrawing()
{
	fill_one_color_buffer();
}
