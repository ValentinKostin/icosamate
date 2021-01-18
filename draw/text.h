#pragma once
#include<map>
#include "ft2build.h"
#include FT_FREETYPE_H

class TextDrawing
{
    FT_Library ft_;
    FT_Face face_;

    struct Character
    {
        unsigned int TextureID; // ID текстуры глифа

        // размер глифа
        unsigned int width_;
        unsigned int height_;

        // смещение от линии шрифта до верхнего/левого угла глифа
        int left_;
        int top_;

        FT_Pos advance_; // смещение до следующего глифа
    };

    std::map<char, Character> Characters;
    void fill_characters();
public:
	TextDrawing();
    ~TextDrawing();
};

TextDrawing& text_drawing();
