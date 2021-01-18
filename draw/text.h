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
        unsigned int TextureID; // ID �������� �����

        // ������ �����
        unsigned int width_;
        unsigned int height_;

        // �������� �� ����� ������ �� ��������/������ ���� �����
        int left_;
        int top_;

        FT_Pos advance_; // �������� �� ���������� �����
    };

    std::map<char, Character> Characters;
    void fill_characters();
public:
	TextDrawing();
    ~TextDrawing();
};

TextDrawing& text_drawing();
