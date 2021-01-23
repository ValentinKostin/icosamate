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
	return bc * 1.7;
}

void add(std::vector<float>& buf, const Coord& c)
{
	buf.push_back(float(c.x_));
	buf.push_back(float(c.y_));
	buf.push_back(float(c.z_));
}
void add(std::vector<float>& buf, const Coord& c0, const Coord& c1, const Coord& c2)
{
	add(buf, c0);
	add(buf, c1);
	add(buf, c2);
}

void assign(std::vector<float>& buf, size_t i, const Coord& c)
{
	buf[i] = (float(c.x_));
	buf[i+1] = (float(c.y_));
	buf[i+2] = (float(c.z_));
}

void IcosamateDrawing::fill_one_color_buffer_faces()
{
	size_t n = IcosamateInSpace::FACE_COUNT;
	one_color_buffer_.reserve(n * 3 * 3);
	for (FaceTriangleId id = 0; id < n; id++)
	{
		const FaceTriangle& t = gic.face_triangle(id);
		add(one_color_buffer_, t.vert_coords(0), t.vert_coords(1), t.vert_coords(2));
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
				add(one_color_buffer_, t.subtriangle_coord(subt_index, pt_index));
	}
}

void IcosamateDrawing::fill_one_color_buffer_not_stickered()
{
	size_t n = IcosamateInSpace::FACE_COUNT;
	one_color_buffer_.reserve(n * 4 * 6 * 3 * 3);
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
				add(one_color_buffer_, c, s, sn);
				add(one_color_buffer_, c, sn, cn);
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
			ColorNum col = j < 3 ? ic_.elem_color(f, ax_id[j]) : f.center_col_;

			for (size_t k = 0; k < 3; k++) // перебор вершин в треугольничках
			{
				if (!colors_only)
					assign(multi_colors_buffer_, buf_ind, t.sticker_coord(j, 2-k));
				
				buf_ind += 3;
				const float* color_buf = &gl_face_colors_[4 * col];
				for (size_t w = 0; w < 4; ++w)
					multi_colors_buffer_[buf_ind++] = color_buf[w];
			}
		}
	}
}

void IcosamateDrawing::fill_axis_coords_buffer()
{
	size_t n = axes().count();
	axis_coords_buffer_.reserve(n * 2 * 3);
	for (AxisId ax_id = 0; ax_id<n; ++ax_id)
	{
		add(axis_coords_buffer_, gic.vertex(ax_id));
		add(axis_coords_buffer_, outer_axis_end(ax_id));
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

void IcosamateDrawing::render_axis_texts()
{
	auto bps = text_drawing_->base_pix_size();
	size_t n = axes().count();
	for (AxisId ax_id = 0; ax_id < n; ++ax_id)
	{
		Coord c = outer_axis_end(ax_id)*1.03;
		auto pix = to_pix(c);
		float scale = 0.67f;
		float pix_x = float(pix.x_) - bps * 0.35f * scale;
		float pix_y = float(pix.y_) - bps * 0.5f * scale;
		text_drawing_->render(std::to_string(ax_id), pix_x, pix_y, scale, axis_color());
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
	fill_axis_coords_buffer();

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

bool open_program(GLuint& pr, const char* sh_fname)
{
	pr = OpenShaderProgram(sh_fname);

#if 0
	GLint num_uniforms = 0;
	glGetProgramiv(pr, GL_ACTIVE_UNIFORMS, &num_uniforms);
	GLchar uniform_name[256];
	GLsizei length;
	GLint size;
	GLenum type;
	for (int i = 0; i < num_uniforms; i++)
	{
		glGetActiveUniform(pr, i, sizeof(uniform_name), &length, &size, &type, uniform_name);
		// ...
	}
#endif

	return pr > 0;
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

void prepare_one_color_drawing(OGLObjs& glo, const float* buf, size_t buffer_bytes_count, size_t one_elem_byte_size, const float* col)
{
	check(open_program(glo.program_, "one_color_poly"));
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

	glo.color_location_ = glGetUniformLocation(glo.program_, "sketchColor");
	if (glo.color_location_ != -1)
		glUniform4fv(glo.color_location_, 1, col);

	// получим индекс вершинного атрибута 'position' из шейдерной программы
	glo.position_location_ = glGetAttribLocation(glo.program_, "position");
	if (glo.position_location_ != -1)
	{
		// укажем параметры вершинного атрибута для текущего активного VBO
		glVertexAttribPointer(glo.position_location_, 3, GL_FLOAT, GL_FALSE, GLsizei(one_elem_byte_size), 0);
		// разрешим использование вершинного атрибута
		glEnableVertexAttribArray(glo.position_location_);
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

	prepare_multi_color_drawing(glo_multi_colors_, multi_colors_buffer(), multi_colors_buffer_bytes_count(), multi_colors_buffer_coord_byte_size());
	prepare_one_color_drawing(glo_vert_one_color_, one_color_buffer(), one_color_buffer_bytes_count(), one_color_buffer_coord_byte_size(), sketch_color());
	prepare_one_color_drawing(glo_axis_, axis_coords_buffer(), axis_coords_buffer_bytes_count(), axis_coords_buffer_coord_byte_size(), axis_color());

	text_drawing_ = create_text_drawing(w_width, w_height);

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
	glDeleteBuffers(1, &glo.vbo_);
	glo.vbo_ = -1;

	// далаем текущий VAO неактивным
	glBindVertexArray(0);
	// удаляем VAO
	glDeleteVertexArrays(1, &glo.vao_);
	glo.vao_ = -1;

	// удаляем шейдерную программу
	ShaderProgramDestroy(glo.program_);
	glo.program_ = -1;

	glo.model_view_projection_matrix_location_ = -1;
	glo.position_location_ = -1;
	glo.color_location_ = -1;
}

void IcosamateDrawing::opengl_clear()
{
	clear(glo_axis_);
	clear(glo_multi_colors_);
	clear(glo_vert_one_color_);
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
	glDrawArrays(GL_TRIANGLES, 0, GLsizei(multi_colors_buffer_coords_count()));
#endif

#if 1
	// рисовка каркаса
	glUseProgram(glo_vert_one_color_.program_);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	set(glo_vert_one_color_, model_view_projection_matrix_);

	glBindVertexArray(glo_vert_one_color_.vao_);
	glDrawArrays(GL_TRIANGLES, 0, GLsizei(one_color_buffer_coords_count()));
#endif

	if (draw_axes())
	{
		glUseProgram(glo_axis_.program_);
		set(glo_axis_, model_view_projection_matrix_);
		glBindVertexArray(glo_axis_.vao_);
		glDrawArrays(GL_LINES, 0, GLsizei(axis_coords_buffer_coords_count()));

		render_axis_texts();
	}

#if 1
	const float turnig_algorithm_color[4] = {1.0f, 0.0f, 0.0f, 1.0f };
	text_drawing_->render(turnig_algorithm_, 50.0f, 50.0f, 1.0f, turnig_algorithm_color);
#endif

	// проверка на ошибки
	OPENGL_CHECK_FOR_ERRORS();
}

void change_angle(float& v, float dv)
{
	v += dv;
	const float two_pi = float(2.0 * M_PI);
	if (v >= two_pi)
		v -= two_pi;
	if (v<0)
		v += two_pi;
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

void IcosamateDrawing::set_rotation_animation(bool r)
{
	rotation_animation_ = r; 
	if (!rotation_animation_)
		start_tick_count_ = 0;
}
