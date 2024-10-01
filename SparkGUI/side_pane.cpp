#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <functional>
#include <iostream>
#include "side_pane.hpp"
#include "rect.hpp"
#include "spark_core.hpp"
#include "stb_easy_font.h"

namespace Spark {
    int _stb_easy_font_print(float x, float y, const char* text, unsigned char color[4], void* vertex_buffer, int vbuf_size) {
        char* vbuf = (char*)vertex_buffer;
        float start_x = x;
        int offset = 0;

        stb_easy_font_color c = { 255,255,255,255 }; // use structure copying to avoid needing depending on memcpy()
        if (color) { c.c[0] = color[0]; c.c[1] = color[1]; c.c[2] = color[2]; c.c[3] = color[3]; }

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

    void print_string(float x, float y, const char* text, float r, float g, float b) {
        static char buffer[99999]; // ~500 chars
        int num_quads;

        num_quads = _stb_easy_font_print(x, y, text, NULL, buffer, sizeof(buffer));

        glColor3f(r, g, b);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, 16, buffer);
        glDrawArrays(GL_QUADS, 0, num_quads * 4);
        glDisableClientState(GL_VERTEX_ARRAY);
    }

    SidePane::SidePane(DockSide side, int size, int padding) : padding(padding) {
        int width, height;
        auto main_window = get_main_window();
        glfwGetWindowSize(main_window, &width, &height);
        switch (side) {
            case START:
                content_bounds = Rect(0, 0, size, height);
                break;
            case TOP:
                content_bounds = Rect(0, 0, width, size);
                break;
            case END:
                content_bounds = Rect(width - size, 0, size, height);
                break;
            case BOTTOM:
                content_bounds = Rect(0, height - size, width, size);
                break;
        };
        using namespace std::placeholders;
        add_mouse_callback(
            std::bind(
                &SidePane::handle_click, this, _1, _2, _3, _4
            )
        );
    }

    void SidePane::print_edit_mode(bool is_edit_mode) {
        int width, height;
        auto main_window = get_main_window();
        glfwGetWindowSize(main_window, &width, &height);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0.0, width, height, 0, 0, 1);

        glScalef(2, 2, 1.5);
        if(is_edit_mode) 
            print_string(60, 220, "edit coordinates mode\nfor primitives: enabled\n(press 1 to switch mode)", 0, 0, 0);
        else
            print_string(60, 220, "edit coordinates mode\nfor primitives: disabled\n(press 1 to switch mode)", 0, 0, 0);
        glPopMatrix();
    }

    void SidePane::render() {
        int width, height;
        auto main_window = get_main_window();
        glfwGetWindowSize(main_window, &width, &height);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0.0, width, height, 0, 0, 1);

        

        glColor3f(0.75, 0.75, 0.75);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(content_bounds.x1, content_bounds.y1);
        glVertex2f(content_bounds.x2, content_bounds.y1);
        glVertex2f(content_bounds.x2, content_bounds.y2);
        glVertex2f(content_bounds.x1, content_bounds.y2);
        glEnd();


        for (auto w : children) {
            w->render();
        }
        glScalef(2, 2, 1.5);
        print_string(60, 10, "activation of a\nnew primitive group(N)", 0, 0, 0);
        print_string(60, 45, "changing the red component\ncolor of the primitive\n(R+ENTER/BACKSPASE to add/sub)", 0, 0, 0);
        print_string(60, 80, "changing the green component\ncolor of the primitive\n(G+ENTER/BACKSPASE to add/sub)", 0, 0, 0);
        print_string(60, 115, "changing the blue component\ncolor of the primitive\n(B+ENTER/BACKSPASE to add/sub)", 0, 0, 0);
        print_string(60, 150, "deleting the last primitive\nin an activated group(D)", 0, 0, 0);
        print_string(60, 185, "deletion of an\nactivated group(DELETE)", 0, 0, 0);
        
        glPopMatrix();
    }
    void SidePane::add_child(Widget *widget) {
        auto sp = get_position();
        auto new_pos = Coordinate {sp.x + margin_start, sp.y + margin_top + children_height};
        widget->set_position(new_pos);

        children_height += widget->get_height() + padding;
        std::cout << "childrens height = " << children_height << std::endl;

        children.push_back(widget);
    }

    bool SidePane::handle_click (GLFWwindow* window, int button, int action, int mods) {
        bool capture = false;
        for (auto c : children) {
            capture |= c->handle_click(window, button, action, mods);
        }
        if (capture) {
            return true;
        }

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
