#pragma once
#include<map>
#include "matrix.h"
#include "glm/glm.hpp"

class TextDrawing
{
    struct Character
    {
        unsigned int texture_id_ = -1; // ID �������� �����

        // ������ �����
        int width_;
        int height_;

        // �������� �� ����� ������ �� ��������/������ ���� �����
        int left_;
        int top_;

        int advance_; // �������� �� ���������� �����
    };

    std::map<char, Character> characters_;
    void fill_characters();

    glm::mat4 projection_;

    int w_width_ = 0, w_height_ = 0;

    unsigned int gl_program_ = -1;
    unsigned int VAO = -1, VBO = -1;

public:
	TextDrawing(int w_width, int w_height);
    void render(const std::string& text, float pix_x, float pix_y, float scale);
};

TextDrawing* create_text_drawing(int w_width, int w_height);
