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
		for (size_t subt_index = 0; subt_index < 4; ++subt_index)
			for (size_t pt_index = 0; pt_index < 3; ++pt_index)
				add_to_one_color_buffer(t.subtriangle_coord(subt_index, pt_index));
	}
}

void IcosamateDrawing::fill_one_color_buffer()
{
	//fill_one_color_buffer_faces();
	fill_one_color_buffer_faces_subtriangles();
}

void IcosamateDrawing::fill_multi_colors_buffer(bool colors_only)
{
	size_t n = IcosamateInSpace::FACE_COUNT;
	if (!colors_only)
		multi_colors_buffer_.resize(n * 4 * 3 * 7);
	size_t buf_ind = 0;
	for (FaceTriangleId id = 0; id < n; id++)
	{
		const FaceTriangle& t = gic.face_triangle(id);
		AxisId ax_id[3] = { t.vert_coords(0).ax_id_, t.vert_coords(1).ax_id_, t.vert_coords(2).ax_id_ };
		const Face& f = ic_.face_by_axis(ax_id[0], ax_id[1], ax_id[2]);

		for (size_t j = 0; j < 4; j++) // перебор треугольничков - элементов
		{
			Color col = j < 3 ? ic_.elem_color(f, ax_id[j]) : f.center_col_;

			for (size_t k = 0; k < 3; k++) // перебор вершин в треугольничках
			{
				const float* crd_buf = &one_color_buffer_[id * 4 * 3 * 3 + j * 3 * 3 + k * 3];
				if (!colors_only)
				{
					for (size_t w = 0; w < 3; ++w)
						multi_colors_buffer_[buf_ind++] = crd_buf[w];
				}
				else
					buf_ind += 3;
				const float* color_buf = &gl_face_colors_[4 * col];
				for (size_t w = 0; w < 4; ++w)
					multi_colors_buffer_[buf_ind++] = color_buf[w];
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
