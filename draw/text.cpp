#include "../def.h"
#include "text.h"
#include "ft2build.h"
#include "opengl.h"
#include FT_FREETYPE_H

TextDrawing::TextDrawing()
{
	check(FT_Init_FreeType(&ft_) == 0);

    check(FT_New_Face(ft_, "fonts/arial.ttf", 0, &face_) == 0);

    fill_characters();
}

TextDrawing::~TextDrawing()
{
    FT_Done_Face(face_);
    FT_Done_FreeType(ft_);
}

void TextDrawing::fill_characters()
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // отключаем ограничение выравнивания байтов

    for (unsigned int c = 0; c < 256; c++)
    {
        // Загружаем глифы символов 
        check(FT_Load_Char(face_, c, FT_LOAD_RENDER) == 0);

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
        Characters.insert({ c, character });
    }
}

TextDrawing& text_drawing()
{
	static TextDrawing td;
	return td;
}
