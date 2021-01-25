#include "../def.h"
#include "draw_def.h"
#include "Shader.h"

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
void add(std::vector<float>& buf, const CoordS& cs)
{
	for (const auto& c : cs)
		add(buf, c);
}

void assign(std::vector<float>& buf, size_t i, const Coord& c)
{
	buf[i] = (float(c.x_));
	buf[i + 1] = (float(c.y_));
	buf[i + 2] = (float(c.z_));
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
	if (buf != nullptr)
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
