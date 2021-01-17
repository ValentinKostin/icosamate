#define _USE_MATH_DEFINES

#include <cmath>
#include "draw_impl.h"
#include "../coord.h"
#include "Shader.h"

IcosomateCoords gic;

IcosamateDrawing& icd()
{
	static IcosamateDrawing icd;
	return icd;
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
			Color col = j < 3 ? ic_.elem_color(f, ax_id[j]) : f.center_col_;

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

double deg_to_rad(double rad)
{
	return rad / 180.0 * M_PI;
}

bool IcosamateDrawing::opengl_init(int w_width, int w_height)
{
	// устанавливаем вьюпорт на все окно
	glViewport(0, 0, w_width, w_height);

	// параметры OpenGL
	const float* cc = clear_color();
	glClearColor(cc[0], cc[1], cc[2], cc[3]);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);

	// создадим и загрузим шейдерные программы
	shaderProgram_colors = OpenShaderProgram("color_poly");
	if (shaderProgram_colors <= 0)
		return false;

	shaderProgram_one_color = OpenShaderProgram("one_color_poly");
	if (shaderProgram_one_color <= 0)
		return false;

	// создадим перспективную матрицу
	const float aspectRatio = (float)w_width / (float)w_height;
	Matrix4Perspective(projectionMatrix, (float)deg_to_rad(22.5), aspectRatio, 0.1f, 100.0f);

	// с помощью видовой матрицы отодвинем сцену назад
	Matrix4Translation(viewMatrix, 0.0f, 0.0f, -8.0f);

	Matrix4Mul(viewProjectionMatrix, projectionMatrix, viewMatrix);

	// запросим у OpenGL свободный индекс VAO
	glGenVertexArrays(1, &vertVAO);
	// сделаем VAO активным
	glBindVertexArray(vertVAO);

	// создадим VBO для данных вершин
	glGenBuffers(1, &vertVBO);
	// начинаем работу с буфером для вершин
	glBindBuffer(GL_ARRAY_BUFFER, vertVBO);
	// поместим в буфер координаты вершин куба
	glBufferData(GL_ARRAY_BUFFER, multi_colors_buffer_bytes_count(), multi_colors_buffer(), GL_STATIC_DRAW);

	// сделаем шейдерную программу активной
	glUseProgram(shaderProgram_colors);
	// получим индекс матрицы из шейдерной программы
	modelViewProjectionMatrixLocation_colors = glGetUniformLocation(shaderProgram_colors, "modelViewProjectionMatrix");

	char* offset = 0;
	// получим индекс вершинного атрибута 'position' из шейдерной программы
	positionLocation_colors = glGetAttribLocation(shaderProgram_colors, "position");
	if (positionLocation_colors != -1)
	{
		// укажем параметры вершинного атрибута для текущего активного VBO
		glVertexAttribPointer(positionLocation_colors, 3, GL_FLOAT, GL_FALSE, GLsizei(multi_colors_buffer_coord_byte_size()), offset);
		// разрешим использование вершинного атрибута
		glEnableVertexAttribArray(positionLocation_colors);
	}

	offset += 3 * sizeof(GLfloat);
	// получим индекс вершинного атрибута 'color' из шейдерной программы
	colorLocation = glGetAttribLocation(shaderProgram_colors, "color");
	if (colorLocation != -1)
	{
		// укажем параметры вершинного атрибута для текущего активного VBO
		glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, GLsizei(multi_colors_buffer_coord_byte_size()), offset);
		// разрешим использование вершинного атрибута
		glEnableVertexAttribArray(colorLocation);
	}

	glUseProgram(shaderProgram_one_color);

	// запросим у OpenGL свободный индекс VAO
	glGenVertexArrays(1, &vertVAO_one_color);
	// сделаем VAO активным
	glBindVertexArray(vertVAO_one_color);
	// создадим VBO для данных вершин
	glGenBuffers(1, &vertVBO_one_color);
	// начинаем работу с буфером для вершин
	glBindBuffer(GL_ARRAY_BUFFER, vertVBO_one_color);
	// поместим в буфер координаты вершин куба
	glBufferData(GL_ARRAY_BUFFER, one_color_buffer_bytes_count(), one_color_buffer(), GL_STATIC_DRAW);

	// получим индекс матрицы из шейдерной программы
	modelViewProjectionMatrixLocation_one_color = glGetUniformLocation(shaderProgram_one_color, "modelViewProjectionMatrix");

	sketchColorLocation = glGetUniformLocation(shaderProgram_one_color, "sketchColor");
	if (sketchColorLocation != -1)
		glUniform4fv(sketchColorLocation, 1, sketch_color());
	sketchColorLocation = glGetUniformLocation(shaderProgram_one_color, "sketchColor");
	if (sketchColorLocation != -1)
		glUniform4fv(sketchColorLocation, 1, sketch_color());

	// получим индекс вершинного атрибута 'position' из шейдерной программы
	positionLocation_one_color = glGetAttribLocation(shaderProgram_one_color, "position");
	if (positionLocation_one_color != -1)
	{
		// укажем параметры вершинного атрибута для текущего активного VBO
		glVertexAttribPointer(positionLocation_one_color, 3, GL_FLOAT, GL_FALSE, GLsizei(one_color_buffer_coord_byte_size()), 0);
		// разрешим использование вершинного атрибута
		glEnableVertexAttribArray(positionLocation_one_color);
	}

	//glLineWidth(2.0);

	// проверим не было ли ошибок
	OPENGL_CHECK_FOR_ERRORS();

	return true;
}

void IcosamateDrawing::opengl_clear()
{
	// ZAGL недописано

	// делаем текущие VBO неактивными
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// удаляем VBO
	glDeleteBuffers(1, &vertVBO);

	// далаем текущий VAO неактивным
	glBindVertexArray(0);
	// удаляем VAO
	glDeleteVertexArrays(1, &vertVAO);

	// удаляем шейдерную программу
	ShaderProgramDestroy(shaderProgram_colors);
}

void IcosamateDrawing::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if 1
	// рисовка цветов треугольничков
	glUseProgram(shaderProgram_colors);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// передаем в шейдер матрицу преобразования координат вершин
	if (modelViewProjectionMatrixLocation_colors != -1)
		glUniformMatrix4fv(modelViewProjectionMatrixLocation_colors, 1, GL_TRUE, modelViewProjectionMatrix);

	// выводим на экран все что относится к VAO
	glBindVertexArray(vertVAO);
	glDrawArrays(GL_TRIANGLES, 0, GLsizei(multi_colors_buffer_coords_count()));
#endif

#if 1
	// рисовка каркаса
	glUseProgram(shaderProgram_one_color);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (modelViewProjectionMatrixLocation_one_color != -1)
		glUniformMatrix4fv(modelViewProjectionMatrixLocation_one_color, 1, GL_TRUE, modelViewProjectionMatrix);

	glBindVertexArray(vertVAO_one_color);
	glDrawArrays(GL_TRIANGLES, 0, GLsizei(one_color_buffer_coords_count()));
#endif

	// проверка на ошибки
	OPENGL_CHECK_FOR_ERRORS();
}

void rotation_angle_change(float* cubeRotation, int scr_ax_num, bool increase, double deltaTime)
{
	float d = 5.0f * (float)deltaTime;
	float n = cubeRotation[scr_ax_num] + (float)(increase ? d : -d);
	float period = 360.0f;
	if (n > period)
		n -= period;
	if (n < -period)
		n += period;
	cubeRotation[scr_ax_num] = n;
}

void IcosamateDrawing::update(double deltaTime)
{
	ASSERT(deltaTime >= 0.0); // проверка на возможность бага

	if (rotation_animation())
		rotation_angle_change(cubeRotation, rotate_animation_screen_axis(), rotation_animation_angle_increase(), deltaTime);

	// рассчитаем матрицу преобразования координат вершин куба	
	Matrix4 modelMatrix; // матрица вращения кубика
	Matrix4Rotation(modelMatrix, cubeRotation[0], cubeRotation[1], cubeRotation[2]);
	Matrix4Mul(modelViewProjectionMatrix, viewProjectionMatrix, modelMatrix);
}

void IcosamateDrawing::set_mode(DrawMode ddm)
{
	draw_mode_ = ddm;
	const float* cc = clear_color();
	glClearColor(cc[0], cc[1], cc[2], cc[3]);
	glUseProgram(shaderProgram_one_color);
	//sketchColorLocation = glGetUniformLocation(shaderProgram_one_color, "sketchColor");
	if (sketchColorLocation != -1)
		glUniform4fv(sketchColorLocation, 1, sketch_color());
}
