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

void IcosamateDrawing::fill_multi_colors_buffer()
{
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
				const float* color_buf = &gl_face_colors_[4 * id];
				multi_colors_buffer_.insert(multi_colors_buffer_.end(), color_buf, color_buf + 4);
			}
		}
	}
}

void IcosamateDrawing::fill_gl_face_colors()
{
	for (const DrawColor& c : draw_colors_)
	{
		gl_face_colors_.push_back(float(((c&0xFF0000) >> 16) / 255.0));
		gl_face_colors_.push_back(float(((c & 0x00FF00) >> 8) / 255.0));
		gl_face_colors_.push_back(float(((c & 0x0000FF)) / 255.0));
		gl_face_colors_.push_back(1.0f);
	}
}

IcosamateDrawing::IcosamateDrawing()
{
	draw_colors_ =
	{
	  0xFFFF00,
	  0x2828FF,
	  0x0000A0,
	  0x800000,
	  0xFF0080,
	  0xFF0000,
	  0x00FF00,
	  0x008000,
	  0x800080,
	  0x00DCB4,
	  0xffffff,
	  0xff00ff,
	  0xffa500,
	  0xffc0cb,
	  0x808000,
	  0x808080,
	  0xadff2f,
	  0xffd700,
	  0xc0c0c0,
	  0x3f3f3f
	};
	fill_gl_face_colors();

	fill_one_color_buffer();
	fill_multi_colors_buffer();
}
