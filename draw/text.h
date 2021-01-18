#pragma once
#include<map>
#include "matrix.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#include "glm/glm.hpp"

class TextDrawing
{
    FT_Library ft_;
    FT_Face face_;

    struct Character
    {
        unsigned int texture_id_ = -1; // ID текстуры глифа

        // размер глифа
        unsigned int width_;
        unsigned int height_;

        // смещение от линии шрифта до верхнего/левого угла глифа
        int left_;
        int top_;

        FT_Pos advance_; // смещение до следующего глифа
    };

    std::map<char, Character> characters_;
    void fill_characters();

    glm::mat4 projection_;

    int w_width_ = 0, w_height_ = 0;

    unsigned int gl_program_ = -1;
    unsigned int VAO = -1, VBO = -1;

public:
	TextDrawing(int w_width, int w_height);
    ~TextDrawing();
    void render(std::string text, float x, float y, float scale);
};

TextDrawing& text_drawing(int w_width, int w_height);
