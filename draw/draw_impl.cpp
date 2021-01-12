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
	one_color_buffer_.reserve(n * 3 * 3);
	for (FaceTriangleId id = 0; id < n; id++)
	{
		const FaceTriangle& t = gic.face_triangle(id);
		add_to_one_color_buffer(t.vert_coords(0));
		add_to_one_color_buffer(t.vert_coords(1));
		add_to_one_color_buffer(t.vert_coords(2));
	}
}

void IcosamateDrawing::fill_one_color_buffer_faces_subtriangles()
{
	size_t n = IcosamateInSpace::FACE_COUNT;
	one_color_buffer_.reserve(n * 4 * 3 * 3);
	for (FaceTriangleId id = 0; id < n; id++)
	{
		const FaceTriangle& t = gic.face_triangle(id);
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

void IcosamateDrawing::fill_one_color_buffer()
{
	//fill_one_color_buffer_faces();
	fill_one_color_buffer_faces_subtriangles();
}

void add_color(std::vector<float>& buf, float color[4])
{
	const float* p = &color[0];
	buf.insert(buf.end(), p, p + 4);
}

void IcosamateDrawing::fill_multi_colors_buffer()
{
	float test_color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

	size_t n = IcosamateInSpace::FACE_COUNT;
	multi_colors_buffer_.reserve(n * 4 * 3 * 7);
	for (FaceTriangleId id = 0; id < n; id++)
	{
		const FaceTriangle& t = gic.face_triangle(id);
		for (size_t j = 0; j < 4; j++) // перебор треугольничков - элементов
		{
			for (size_t k = 0; k < 3; k++) // перебор вершин в треугольничках
			{
				const float* crd_buf = &one_color_buffer_[id * 4 * 3 * 3 + j * 3 * 3 + k * 3];
				multi_colors_buffer_.insert(multi_colors_buffer_.end(), crd_buf, crd_buf + 3);
				add_color(multi_colors_buffer_, test_color);
			}
		}
	}
}

IcosamateDrawing::IcosamateDrawing()
{
	fill_one_color_buffer();
	fill_multi_colors_buffer();
}
