#define _USE_MATH_DEFINES

#include <cmath>
#include "draw_impl.h"
#include "../coord.h"
#include "Shader.h"
#include "text.h"
#include "matrix.h"

IcosomateCoords gic;

IcosamateDrawing& icd()
{
	static IcosamateDrawing icd;
	return icd;
}

Coord outer_axis_end(AxisId ax_id)
{
	Coord bc = gic.vertex(ax_id);
	return bc * 1.68;
}

void IcosamateDrawing::fill_one_color_buffer_faces()
{
	size_t n = IcosamateInSpace::FACE_COUNT;
	std::vector<float>& buf = one_color_buffer_.buf_;
	buf.reserve(n * 3 * 3);
	for (FaceTriangleId id = 0; id < n; id++)
	{
		const FaceTriangle& t = gic.face_triangle(id);
		add(buf, t.vert_coords(0), t.vert_coords(1), t.vert_coords(2));
	}
}

void IcosamateDrawing::fill_one_color_buffer_faces_subtriangles()
{
	size_t n = IcosamateInSpace::FACE_COUNT;
	std::vector<float>& buf = one_color_buffer_.buf_;
	buf.reserve(n * 4 * 3 * 3);
	for (FaceTriangleId id = 0; id < n; id++)
	{
		const FaceTriangle& t = gic.face_triangle(id);
		for (size_t subt_index = 0; subt_index < 4; ++subt_index)
			for (size_t pt_index = 0; pt_index < 3; ++pt_index)
				add(buf, t.subtriangle_coord(subt_index, pt_index));
	}
}

void IcosamateDrawing::fill_one_color_buffer_not_stickered()
{
	size_t n = IcosamateInSpace::FACE_COUNT;
	std::vector<float>& buf = one_color_buffer_.buf_;
	buf.reserve(n * 4 * 6 * 3 * 3);
	for (FaceTriangleId id = 0; id < n; id++)
	{
		const FaceTriangle& t = gic.face_triangle(id);
		for (size_t subt_index = 0; subt_index < 4; ++subt_index)
			for (size_t pt_index = 0; pt_index < 3; ++pt_index)
			{
				const Coord& c = t.subtriangle_coord(subt_index, pt_index);
				const Coord& s = t.sticker_coord(subt_index, pt_index);
				const Coord& cn = t.subtriangle_coord(subt_index, (pt_index + 1) % 3);
				const Coord& sn = t.sticker_coord(subt_index, (pt_index + 1) % 3);
				add(buf, c, s, sn);
				add(buf, c, sn, cn);
			}				
	}
}

void IcosamateDrawing::fill_one_color_buffer()
{
	fill_one_color_buffer_not_stickered();
}

void IcosamateDrawing::fill_multi_colors_buffer(bool colors_only)
{
	size_t n = IcosamateInSpace::FACE_COUNT;
	std::vector<float>& buf = multi_colors_buffer_.buf_;
	if (!colors_only)
		buf.resize(n * 4 * 3 * 7);
	size_t buf_ind = 0;
	for (FaceTriangleId id = 0; id < n; id++)
	{
		const FaceTriangle& t = gic.face_triangle(id);
		AxisId ax_id[3] = { t.vert_coords(0).ax_id_, t.vert_coords(1).ax_id_, t.vert_coords(2).ax_id_ };
		const Face& f = ic_.face_by_axis(ax_id[0], ax_id[1], ax_id[2]);

		for (size_t j = 0; j < 4; j++) // перебор треугольничков - элементов
		{
			ColorNum col = j < 3 ? ic_.elem_color(f, ax_id[j]) : f.center_col_;

			for (size_t k = 0; k < 3; k++) // перебор вершин в треугольничках
			{
				if (!colors_only)
					assign(buf, buf_ind, t.sticker_coord(j, 2-k));
				
				buf_ind += 3;
				const float* color_buf = &gl_face_colors_[4 * col];
				for (size_t w = 0; w < 4; ++w)
					buf[buf_ind++] = color_buf[w];
			}
		}
	}
}

void IcosamateDrawing::fill_axis_coords_buffer()
{
	size_t n = axes().count();
	std::vector<float>& buf = axis_coords_buffer_.buf_;
	buf.reserve(n * 2 * 3);
	for (AxisId ax_id = 0; ax_id<n; ++ax_id)
	{
		add(buf, gic.vertex(ax_id));
		add(buf, outer_axis_end(ax_id));
	}
}

CoordS IcosamateDrawing::define_arc(AxisId ax_id_1, AxisId ax_id_2)
{
	double r = gic.radius();
	const Axes& aa = axes();
	if (ax_id_1 != ax_id_2)
	{
		const Axis& a1 = aa.axis(ax_id_1);
		const Axis& a2 = aa.axis(ax_id_2);
		if (a1.opposite_id_ != a2.id_)
			return define_arc_on_sphere(gic.vertex(ax_id_1), gic.vertex(ax_id_2), r);

		AxisId ax_id_pr_1 = a1.near_axes_[0];
		const Axis& a1_pr = aa.axis(ax_id_pr_1);
		std::vector<AxisId> cm_ax = aa.near_common_axis(ax_id_pr_1, ax_id_2);
		check(!cm_ax.empty());
		Coord c_mid = sphere_middle_point(gic.vertex(ax_id_pr_1), gic.vertex(cm_ax[0]), r);
		CoordS cs1 = define_arc_on_sphere(gic.vertex(ax_id_1), c_mid, r);
		check(!cs1.empty());
		CoordS cs2 = define_arc_on_sphere(c_mid, gic.vertex(ax_id_2), r);
		cs1.pop_back();
		cs1.insert(cs1.end(), cs2.begin(), cs2.end());
		return cs1;
	}
	return CoordS();
}

void IcosamateDrawing::fill_ve_arrows_coords()
{
	ve_arrows_.clear_coords();
	for (VertexId vid=0; vid<Icosamate::VERTICES_COUNT; ++vid)
	{
		AxisId ax_id_1 = ic0_.axis_by_vertex(vid);
		AxisId ax_id_2 = ic_.axis_by_vertex(vid);
		if (ax_id_1 != ax_id_2)
			ve_arrows_.add_arrow(define_arc(ax_id_1, ax_id_2));
	}
	ve_arrows_.complete();
}

void IcosamateDrawing::fill_ve_rotation_arrows_coords()
{
	ve_rotation_arrows_.clear_coords();
	for (VertexId vid = 0; vid < Icosamate::VERTICES_COUNT; ++vid)
	{
		for (const Axis& a : axes().axes())
		{
			VertexElem ve1 = ic0_.vertex_elem_by_axis(a.id_);
			VertexElem ve2 = ic_.vertex_elem_by_axis(a.id_);
			if (ve1.id_ == ve2.id_)
			{
				VertexId vid = ve1.id_;
				const Face& f1 = ic0_.face_by_axis(a.id_, a.near_axes_[0], a.near_axes_[1]);
				const Face& f2 = ic_.face_by_axis(a.id_, a.near_axes_[0], a.near_axes_[1]);
				size_t ind1 = f1.vert_elems_colors_inds_[f1.index(vid)];
				size_t ind2 = f2.vert_elems_colors_inds_[f2.index(vid)];
				if (ind1 != ind2)
				{
					size_t diff = ((5 + ind2) - ind1) % 5;
					double angle = M_PI * (2.0 / 5.0) * (diff > 2 ? 5 - diff : diff);
					double clockwise = diff <= 2;
					const auto& c0 = gic.vertex(a.id_);
					const auto& c1 = gic.vertex(a.near_axes_[0]);
					const auto& c2 = gic.vertex(a.near_axes_[1]);
					Coord c = (c0 + c1 + c2) * (1.0 / 3.0);
					double r = gic.radius()*0.25;
					ve_rotation_arrows_.add_arrow(define_arc_around_axis(c0, c, angle, clockwise, r));
				}
			}
		}
	}
	ve_rotation_arrows_.complete();
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

void IcosamateDrawing::render_axis_texts()
{
	auto bps = text_drawing_->base_pix_size();
	const Axes& aa = axes();
	size_t n = aa.count();
	for (AxisId ax_id = 0; ax_id < n; ++ax_id)
	{
		Coord c = outer_axis_end(ax_id)*1.03;
		auto pix = to_pix(c);
		float scale = 0.67f;
		float pix_x = float(pix.x_) - bps * 0.35f * scale;
		float pix_y = float(pix.y_) - bps * 0.5f * scale;
		std::string text { aa.get_char(ax_id) };
		text_drawing_->render(text, pix_x, pix_y, scale, axis_color());
	}
}

GLPix IcosamateDrawing::to_pix(const Coord& c) const
{
	glm::vec4 v(c.x_, c.y_, c.z_, 1.0f);
	auto v2 = model_view_projection_matrix_ * v;

	return
	{
	   (w_width_ *  (v2[0] / v2[3] + 1))/2.0,
	   (w_height_ * (v2[1] / v2[3] + 1)) / 2.0,
	   ((v2[2] / v2[3]) + 1) / 2.0,
	};
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
	  0xCC0000,
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
	ve_arrows_.set_color(ve_arrows_color_);
	ve_rotation_arrows_.set_color(ve_rotation_arrows_color_);
	ve_rotation_arrows_.set_arrow_end(0.1);

	fill_one_color_buffer();
	fill_multi_colors_buffer();
	fill_axis_coords_buffer();
	fill_ve_arrows_coords();
	fill_ve_rotation_arrows_coords();

	glm::mat4 rm = glm::rotate(glm::mat4(1.0f), float(M_PI_2), glm::vec3(1.0, 0.0, 0.0));
	model_matrix_ = glm::rotate(rm, -float(M_PI), glm::vec3(0.0, 1.0, 0.0));
}

IcosamateDrawing::~IcosamateDrawing()
{
	delete text_drawing_;
}

double deg_to_rad(double rad)
{
	return rad / 180.0 * M_PI;
}
void prepare_multi_color_drawing(OGLObjs& glo, const float* buf, size_t buffer_bytes_count, size_t one_elem_byte_size)
{
	check(open_program(glo.program_, "color_poly"));
	glUseProgram(glo.program_);

	// запросим у OpenGL свободный индекс VAO
	glGenVertexArrays(1, &glo.vao_);
	// сделаем VAO активным
	glBindVertexArray(glo.vao_);
	// создадим VBO для данных вершин
	glGenBuffers(1, &glo.vbo_);
	// начинаем работу с буфером для вершин
	glBindBuffer(GL_ARRAY_BUFFER, glo.vbo_);
	glBufferData(GL_ARRAY_BUFFER, buffer_bytes_count, buf, GL_STATIC_DRAW);

	// получим индекс матрицы из шейдерной программы
	glo.model_view_projection_matrix_location_ = glGetUniformLocation(glo.program_, "modelViewProjectionMatrix");

	char* offset = 0;
	// получим индекс вершинного атрибута 'position' из шейдерной программы
	glo.position_location_ = glGetAttribLocation(glo.program_, "position");
	if (glo.position_location_ != -1)
	{
		// укажем параметры вершинного атрибута для текущего активного VBO
		glVertexAttribPointer(glo.position_location_, 3, GL_FLOAT, GL_FALSE, GLsizei(one_elem_byte_size), 0);
		// разрешим использование вершинного атрибута
		glEnableVertexAttribArray(glo.position_location_);
	}

	offset += 3 * sizeof(GLfloat);
	// получим индекс вершинного атрибута 'color' из шейдерной программы
	glo.color_location_ = glGetAttribLocation(glo.program_, "color");
	if (glo.color_location_ != -1)
	{
		// укажем параметры вершинного атрибута для текущего активного VBO
		glVertexAttribPointer(glo.color_location_, 4, GL_FLOAT, GL_FALSE, GLsizei(one_elem_byte_size), offset);
		// разрешим использование вершинного атрибута
		glEnableVertexAttribArray(glo.color_location_);
	}
}

bool IcosamateDrawing::opengl_init(int w_width, int w_height)
{
	w_width_ = w_width; w_height_ = w_height;
	// устанавливаем вьюпорт на все окно
	glViewport(0, 0, w_width, w_height);

	// параметры OpenGL
	const float* cc = clear_color();
	glClearColor(cc[0], cc[1], cc[2], cc[3]);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);

	// создадим перспективную матрицу
	const float aspectRatio = (float)w_width / (float)w_height;
	projection_matrix_ = glm::perspective((float)deg_to_rad(22.5), aspectRatio, 0.1f, 100.0f);

	// с помощью видовой матрицы отодвинем сцену назад
	view_matrix_ = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, -9.0f));
	view_projection_matrix_ = projection_matrix_ * view_matrix_;

	prepare_multi_color_drawing(glo_multi_colors_, multi_colors_buffer_.ptr(), multi_colors_buffer_.bytes_count(), multi_colors_buffer_.elem_byte_size());
	prepare_one_color_drawing(glo_vert_one_color_, one_color_buffer_.ptr(), one_color_buffer_.bytes_count(), one_color_buffer_.elem_byte_size(), sketch_color());
	prepare_one_color_drawing(glo_axis_, axis_coords_buffer_.ptr(), axis_coords_buffer_.bytes_count(), axis_coords_buffer_.elem_byte_size(), axis_color());

	text_drawing_ = create_text_drawing(w_width, w_height);

	ve_arrows_.gl_init();
	ve_rotation_arrows_.gl_init();

	// проверим не было ли ошибок
	OPENGL_CHECK_FOR_ERRORS();

	return true;
}

void clear(OGLObjs& glo)
{
	// делаем текущие VBO неактивными
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// удаляем VBO
	if (glo.vbo_!=-1)
		glDeleteBuffers(1, &glo.vbo_);
	glo.vbo_ = -1;

	// далаем текущий VAO неактивным
	glBindVertexArray(0);
	// удаляем VAO
	if (glo.vao_ != -1)
		glDeleteVertexArrays(1, &glo.vao_);
	glo.vao_ = -1;

	// удаляем шейдерную программу
	if (glo.program_ > 0)
		ShaderProgramDestroy(glo.program_);
	glo.program_ = -1;

	glo.model_view_projection_matrix_location_ = -1;
	glo.position_location_ = -1;
	glo.color_location_ = -1;
}

OGLObjs::~OGLObjs()
{
	clear(*this);
}

void IcosamateDrawing::opengl_clear()
{
	clear(glo_axis_);
	clear(glo_multi_colors_);
	clear(glo_vert_one_color_);

	delete text_drawing_;
	text_drawing_ = nullptr;

	ve_arrows_.clear();
	ve_rotation_arrows_.clear();
}

void set(OGLObjs& glo, const glm::mat4& model_view_projection_matrix)
{
	if (glo.model_view_projection_matrix_location_ != -1)
		glUniformMatrix4fv(glo.model_view_projection_matrix_location_, 1, GL_FALSE, glm::value_ptr(model_view_projection_matrix));
}

void IcosamateDrawing::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if 1
	// рисовка цветов треугольничков
	glUseProgram(glo_multi_colors_.program_);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	set(glo_multi_colors_, model_view_projection_matrix_);

	// выводим на экран все что относится к VAO
	glBindVertexArray(glo_multi_colors_.vao_);
	glDrawArrays(GL_TRIANGLES, 0, GLsizei(multi_colors_buffer_.elems_count()));
#endif

#if 1
	// рисовка каркаса
	glUseProgram(glo_vert_one_color_.program_);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	set(glo_vert_one_color_, model_view_projection_matrix_);

	glBindVertexArray(glo_vert_one_color_.vao_);
	glDrawArrays(GL_TRIANGLES, 0, GLsizei(one_color_buffer_.elems_count()));
#endif

	if (draw_axes())
	{
		glUseProgram(glo_axis_.program_);
		set(glo_axis_, model_view_projection_matrix_);
		glBindVertexArray(glo_axis_.vao_);
		glDrawArrays(GL_LINES, 0, GLsizei(axis_coords_buffer_.elems_count()));

		render_axis_texts();
	}

#if 1
	const float turnig_algorithm_color[4] = {1.0f, 0.0f, 0.0f, 1.0f };
	text_drawing_->render(turnig_algorithm_, 50.0f, 50.0f, 1.0f, turnig_algorithm_color);
#endif

	ve_arrows_.render(model_view_projection_matrix_);
	ve_rotation_arrows_.render(model_view_projection_matrix_);

	// проверка на ошибки
	OPENGL_CHECK_FOR_ERRORS();
}

void IcosamateDrawing::update()
{
	if (rotation_animation())
	{
		size_t r = GetTickCount64();
		if (start_tick_count_ == 0)
			start_tick_count_ = r;
		size_t time_ms = r - start_tick_count_;
		const size_t FULL_ROTATION_MS = 10000;
		time_ms = time_ms % FULL_ROTATION_MS;
		float angle = float((time_ms * 2.0 * M_PI) / FULL_ROTATION_MS);
		float rotation_angle = rotation_animation_angle_increase() ? angle : -angle;
		start_tick_count_ = r;

		int axis = rotate_animation_screen_axis();
		glm::vec3 ax(0);
		ax[axis] = 1.0;
		glm::mat4 rot_matrix = glm::rotate(glm::mat4(1.0f), rotation_angle, ax);
		model_matrix_ = rot_matrix * model_matrix_;
	}
	model_view_projection_matrix_ = view_projection_matrix_ * model_matrix_;
}

void IcosamateDrawing::set_mode(DrawMode ddm)
{
	draw_mode_ = ddm;
	const float* cc = clear_color();
	glClearColor(cc[0], cc[1], cc[2], cc[3]);
	glUseProgram(glo_vert_one_color_.program_);
	if (glo_vert_one_color_.color_location_ != -1)
		glUniform4fv(glo_vert_one_color_.color_location_, 1, sketch_color());
}

void IcosamateDrawing::update_draw_buffers()
{
	fill_multi_colors_buffer(true);
	::complete(glo_multi_colors_, multi_colors_buffer_);
	fill_ve_arrows_coords();
	fill_ve_rotation_arrows_coords();
}

void IcosamateDrawing::set_icosomate(const IcosamateInSpace& ic)
{
	ic_ = ic; 
	update_draw_buffers();
}

void IcosamateDrawing::inverse()
{
	TurnAlg ta = ::inverse(turnig_algorithm_);
	set_turnig_algorithm(ta);
	IcosamateInSpace ic;
	ic.actions(IcosamateInSpace::from_str(ta));
	set_icosomate(ic);
}

void IcosamateDrawing::set_rotation_animation(bool r)
{
	rotation_animation_ = r; 
	if (!rotation_animation_)
		start_tick_count_ = 0;
}

void IcosamateDrawing::set_all_arrows_visible(bool visible)
{
	ve_arrows_.set_visible(visible);
	ve_rotation_arrows_.set_visible(visible);
}

bool IcosamateDrawing::is_arrows_visible(ArrowsType at) const
{
	if (at == ArrowsType::VertElems)
		return ve_arrows_.visible();
	else if (at == ArrowsType::VertElemsRotations)
		return ve_rotation_arrows_.visible();
	else
		return false;
}

void IcosamateDrawing::turn(char ax_name, bool clockwise)
{
	const Axes& aa = axes();
	AxisId ax_id = aa.get_axis(ax_name);
	Action a = ic_.turn_action(ax_id, clockwise);
	ic_.action(a);

	turnig_algorithm_.push_back(ax_name);
	if (!clockwise)
		turnig_algorithm_.push_back('\'');

	update_draw_buffers();
}

void IcosamateDrawing::move(char ax_name, bool clockwise)
{
	const Axes& aa = axes();
	AxisId ax_id = aa.get_axis(ax_name);
	Action a = ic_.move_action(ax_id, clockwise);
	ic_.action(a);

	turnig_algorithm_.push_back('M');
	turnig_algorithm_.push_back(ax_name);
	if (!clockwise)
		turnig_algorithm_.push_back('\'');

	update_draw_buffers();
}

void IcosamateDrawing::undo()
{
	if (turnig_algorithm_.empty())
		return;
	std::string new_ta = turnig_algorithm_;
	bool clockwise = new_ta.back() == '\'';
	if (clockwise)
		new_ta.pop_back();
	check(!new_ta.empty());
	const Axes& aa = axes();
	char ac = new_ta.back();
	if (!aa.is_axis_char(ac))
		return;
	AxisId ax_id = aa.get_axis(ac);
	new_ta.pop_back();
	size_t n = new_ta.size();
	bool was_move = n > 0 && new_ta[n - 1] == 'M';
	if (was_move)
		new_ta.pop_back();

	Action inv_a = was_move ? IcosamateInSpace::move_action(ax_id, clockwise) : IcosamateInSpace::turn_action(ax_id, clockwise);
	ic_.action(inv_a);
	turnig_algorithm_.swap(new_ta);

	update_draw_buffers();
}
