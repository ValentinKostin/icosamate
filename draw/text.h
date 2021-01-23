#pragma once
#include<map>
#include "matrix.h"

class TextDrawing
{
    struct Character
    {
        unsigned int texture_id_ = -1; // ID текстуры глифа

        // размер глифа
        int width_;
        int height_;

        // смещение от линии шрифта до верхнего/левого угла глифа
        int left_;
        int top_;

        int advance_; // смещение до следующего глифа
    };

    std::map<char, Character> characters_;
    void fill_characters();

    glm::mat4 projection_;

    int w_width_ = 0, w_height_ = 0;

    unsigned int gl_program_ = -1;
    unsigned int VAO = -1, VBO = -1;

    const size_t base_pix_size_ = 48;

public:
	TextDrawing(int w_width, int w_height);
    void render(const std::string& text, float pix_x, float pix_y, float scale, const float color[4]);
    size_t base_pix_size() const { return base_pix_size_; }
};

TextDrawing* create_text_drawing(int w_width, int w_height);
