#include "../def.h"
#include "text.h"
#include "ft2build.h"
#include "opengl.h"
#include "Shader.h"
#include FT_FREETYPE_H
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#undef max

TextDrawing::TextDrawing(int w_width, int w_height) : w_width_(w_width), w_height_(w_height)
{
	check(FT_Init_FreeType(&ft_) == 0);

    check(FT_New_Face(ft_, get_data_full_path("DejaVuSans.ttf").c_str(), 0, &face_) == 0);
    FT_Set_Pixel_Sizes(face_, 0, 48);

    fill_characters();

    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    gl_program_ = OpenShaderProgram("text");
	check(gl_program_ > 0);
	glUseProgram(gl_program_);

	GLint projectionId = glGetUniformLocation(gl_program_, "projection");
	projection_ = glm::ortho(0.0f, (float)w_width_, 0.0f, (float)w_height_);
    glUniformMatrix4fv(projectionId, 1, GL_FALSE, glm::value_ptr(projection_));

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

TextDrawing::~TextDrawing()
{
    FT_Done_Face(face_);
    FT_Done_FreeType(ft_);
}

void TextDrawing::fill_characters()
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // отключаем ограничение выравнивания байтов

    for (unsigned int cc = 0; cc<256; cc++)
    {
        char c = (char)cc;
        // Загружаем глифы символов 
        if (FT_Load_Char(face_, c, FT_LOAD_RENDER) != 0)
            continue;

        // Генерируем текстуру
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face_->glyph->bitmap.width,
            face_->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face_->glyph->bitmap.buffer
        );

        // Задаем для текстуры необходимые опции
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Сохраняем символ для последующего использования
        Character character = 
        {
            texture,
            face_->glyph->bitmap.width, face_->glyph->bitmap.rows,
            face_->glyph->bitmap_left, face_->glyph->bitmap_top,
            face_->glyph->advance.x
        };
        characters_.insert({ c, character });
    }
}

void TextDrawing::render(const std::string& text, float x, float y, float scale)
{
    float color[3] = {1.0f,0.0f,0.0f}; // ZAGL

    // Активируем соответствующее состояние рендеринга	
    glUseProgram(gl_program_);
    glUniform3f(glGetUniformLocation(gl_program_, "textColor"), color[0], color[1], color[2]);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // Перебираем все символы
    for (char c : text)
    {
        const Character& ch = characters_.at(c);

        float xpos = x + ch.left_ * scale;
        float ypos = y - (ch.height_ - ch.top_) * scale;

        float w = ch.width_ * scale;
        float h = ch.height_ * scale;

        // Обновляем VBO для каждого символа
        float vertices[6][4] = {
            { xpos, ypos + h, 0.0f, 0.0f },
            { xpos, ypos, 0.0f, 1.0f },
            { xpos + w, ypos, 1.0f, 1.0f },

            { xpos, ypos + h, 0.0f, 0.0f },
            { xpos + w, ypos, 1.0f, 1.0f },
            { xpos + w, ypos + h, 1.0f, 0.0f }
        };

        // Визуализируем текстуру глифа поверх прямоугольника
        glBindTexture(GL_TEXTURE_2D, ch.texture_id_);

        // Обновляем содержимое памяти VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Рендерим прямоугольник
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Теперь производим смещение для отображения следующего глифа (обратите внимание, что данное смещение измеряется в единицах, составляющих 1/64 пикселя)
        x += ch.advance_ * (scale/64.0f);
	}
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

TextDrawing* create_text_drawing(int w_width, int w_height)
{
	return new TextDrawing(w_width, w_height);
}
