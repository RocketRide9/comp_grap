#include "label.hpp"
#include <GL/gl.h>
#include "stb_easy_font.h"

namespace Spark {
    std::shared_ptr<Label> Label::create(LabelSchema schema) {
        auto res = std::shared_ptr<Label>(new Label);

        if (schema.bind) {
            *schema.bind = std::shared_ptr<Label>(res);
        }
        res->margin = schema.margin;
        res->content_bounds = Rect(0, 0, schema.width, schema.height);
        res->text = schema.text;

        return res;
    }

    void Label::set_text(std::string &text) {
        this->text = text;
    }
    std::string Label::get_text() {
        return this->text;
    }


    int _stb_easy_font_print(float x, float y, const char* text, unsigned char color[4], void* vertex_buffer, int vbuf_size) {
        char* vbuf = (char*)vertex_buffer;
        float start_x = x;
        int offset = 0;

        stb_easy_font_color c = { 255,255,255,255 }; // use structure copying to avoid needing depending on memcpy()
        if (color) {
            c.c[0] = color[0];
            c.c[1] = color[1];
            c.c[2] = color[2];
            c.c[3] = color[3];
        }

        while (*text && offset < vbuf_size) {
            if (*text == '\n') {
                y += 12;
                x = start_x;
            }
            else {
                unsigned char advance = stb_easy_font_charinfo[*text - 32].advance;
                float y_ch = advance & 16 ? y + 1 : y;
                int h_seg, v_seg, num_h, num_v;
                h_seg = stb_easy_font_charinfo[*text - 32].h_seg;
                v_seg = stb_easy_font_charinfo[*text - 32].v_seg;
                num_h = stb_easy_font_charinfo[*text - 32 + 1].h_seg - h_seg;
                num_v = stb_easy_font_charinfo[*text - 32 + 1].v_seg - v_seg;
                offset = stb_easy_font_draw_segs(x, y_ch, &stb_easy_font_hseg[h_seg], num_h, 0, c, vbuf, vbuf_size, offset);
                offset = stb_easy_font_draw_segs(x, y_ch, &stb_easy_font_vseg[v_seg], num_v, 1, c, vbuf, vbuf_size, offset);
                x += advance & 15;
                x += stb_easy_font_spacing_val;
            }
            ++text;
        }
        return (unsigned)offset / 64;
    }
    void Label::render() {
        int win_width, win_height;
        auto main_window = get_main_window();
        glfwGetWindowSize(main_window, &win_width, &win_height);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0.0, win_width, win_height, 0, 0, 1);

        // print_string(60, 10, "activation of a\nnew primitive group(N)", 0, 0, 0);

        static char buffer[99999]; // ~500 chars
        int num_quads;

        num_quads = _stb_easy_font_print(
            0,
            0,
            text.c_str(),
            NULL,
            buffer, sizeof(buffer)
        );

        glTranslatef(content_bounds.x1, content_bounds.y1, 0);
        glScalef(2, 2, 1.5);
        glColor3f(0, 0, 0);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, 16, buffer);
        glDrawArrays(GL_QUADS, 0, num_quads * 4);
        glDisableClientState(GL_VERTEX_ARRAY);

        glPopMatrix();
    }

    bool Label::handle_click(GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            double _x, _y;
            glfwGetCursorPos(window, &_x, &_y);

            if (content_bounds.contains(_x, _y)) {
                return true;
            }
        }
        return false;
    }
}
