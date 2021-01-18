#include "../def.h"
#include "text.h"
#include "ft2build.h"
#include "opengl.h"
#include FT_FREETYPE_H

TextDrawing::TextDrawing()
{
	check(FT_Init_FreeType(&ft_) == 0);

    check(FT_New_Face(ft_, "fonts/arial.ttf", 0, &face_) == 0); // ZAGL

    fill_characters();

    //glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);  // ZAGL

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ZAGL �������� ��������
}

TextDrawing::~TextDrawing()
{
    FT_Done_Face(face_);
    FT_Done_FreeType(ft_);
}

void TextDrawing::fill_characters()
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // ��������� ����������� ������������ ������

    for (unsigned int c = 0; c < 256; c++)
    {
        // ��������� ����� �������� 
        check(FT_Load_Char(face_, c, FT_LOAD_RENDER) == 0);

        // ���������� ��������
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

        // ������ ��� �������� ����������� �����
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // ��������� ������ ��� ������������ �������������
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

void TextDrawing::render(std::string text, float x, float y, float scale)
{
    float color[3] = {1.0f,0.0f,0.0f}; // ZAGL

    // ���������� ��������������� ��������� ����������	
    glUseProgram(gl_program_);
    glUniform3f(glGetUniformLocation(gl_program_, "textColor"), color[0], color[1], color[2]);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // ���������� ��� �������
    for (auto c = text.begin(); c != text.end(); c++)
    {
        Character ch = characters_[*c];

        float xpos = x + ch.width_ * scale;
        float ypos = y - (ch.height_ - ch.top_) * scale;

        float w = ch.width_ * scale;
        float h = ch.height_ * scale;

        // ��������� VBO ��� ������� �������
        float vertices[6][4] = {
            { xpos, ypos + h, 0.0f, 0.0f },
            { xpos, ypos, 0.0f, 1.0f },
            { xpos + w, ypos, 1.0f, 1.0f },

            { xpos, ypos + h, 0.0f, 0.0f },
            { xpos + w, ypos, 1.0f, 1.0f },
            { xpos + w, ypos + h, 1.0f, 0.0f }
        };

        // ������������� �������� ����� ������ ��������������
        glBindTexture(GL_TEXTURE_2D, ch.texture_id_);

        // ��������� ���������� ������ VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // �������� �������������
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // ������ ���������� �������� ��� ����������� ���������� ����� (�������� ��������, ��� ������ �������� ���������� � ��������, ������������ 1/64 �������)
        x += ((unsigned int)(ch.advance_) >> 6) * scale; // ��������� ����� �� 6, ����� �������� �������� � �������� (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

TextDrawing& text_drawing()
{
	static TextDrawing td;
	return td;
}
