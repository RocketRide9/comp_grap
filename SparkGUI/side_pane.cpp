#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <functional>
#include <memory>
#include "side_pane.hpp"
#include "rect.hpp"
#include "spark_core.hpp"
#include "stb_easy_font.h"

namespace Spark {
    std::shared_ptr<SidePane> SidePane::create(SidePaneSchema schema) {
        auto res = std::shared_ptr<SidePane>(new SidePane);

        if (schema.bind) {
            *schema.bind = std::shared_ptr<SidePane>(res);
        }

        int width, height;
        auto main_window = get_main_window();
        glfwGetWindowSize(main_window, &width, &height);
        switch (schema.side) {
            case START:
                res->content_bounds = Rect(0, 0, schema.size, height);
                break;
            case TOP:
                res->content_bounds = Rect(0, 0, width, schema.size);
                break;
            case END:
                res->content_bounds = Rect(width - schema.size, 0, schema.size, height);
                break;
            case BOTTOM:
                res->content_bounds = Rect(0, height - schema.size, width, schema.size);
                break;
        };

        res->set_child(schema.child);

        using namespace std::placeholders;
        add_mouse_callback(
            std::bind(
                &SidePane::handle_click, res, _1, _2, _3, _4
            )
        );

        return res;
    }


    // SidePane::SidePane(DockSide side, int size) {
    //     int width, height;
    //     auto main_window = get_main_window();
    //     glfwGetWindowSize(main_window, &width, &height);
    //     switch (side) {
    //         case START:
    //             content_bounds = Rect(0, 0, size, height);
    //             break;
    //         case TOP:
    //             content_bounds = Rect(0, 0, width, size);
    //             break;
    //         case END:
    //             content_bounds = Rect(width - size, 0, size, height);
    //             break;
    //         case BOTTOM:
    //             content_bounds = Rect(0, height - size, width, size);
    //             break;
    //     };
    //     using namespace std::placeholders;
    //     add_mouse_callback(
    //         std::bind(
    //             &SidePane::handle_click, this, _1, _2, _3, _4
    //         )
    //     );
    // }

    // void SidePane::print_edit_mode(bool is_edit_mode) {
    //     int width, height;
    //     auto main_window = get_main_window();
    //     glfwGetWindowSize(main_window, &width, &height);
    //     glMatrixMode(GL_MODELVIEW);
    //     glPushMatrix();
    //     glLoadIdentity();
    //     glOrtho(0.0, width, height, 0, 0, 1);

    //     glScalef(2, 2, 1.5);
    //     if(is_edit_mode)
    //         print_string(60, 220, "edit coordinates mode\nfor primitives: enabled\n(press 1 to switch mode)", 0, 0, 0);
    //     else
    //         print_string(60, 220, "edit coordinates mode\nfor primitives: disabled\n(press 1 to switch mode)", 0, 0, 0);
    //     glPopMatrix();
    // }

    void SidePane::render() {
        int width, height;
        auto main_window = get_main_window();
        glfwGetWindowSize(main_window, &width, &height);
        glMatrixMode(GL_MODELVIEW);
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

        glPopMatrix();

        child->render();
        // glScalef(2, 2, 1.5);
        // print_string(60, 10, "activation of a\nnew primitive group(N)", 0, 0, 0);
        // print_string(60, 45, "changing the red component\ncolor of the primitive\n(R+ENTER/BACKSPASE to add/sub)", 0, 0, 0);
        // print_string(60, 80, "changing the green component\ncolor of the primitive\n(G+ENTER/BACKSPASE to add/sub)", 0, 0, 0);
        // print_string(60, 115, "changing the blue component\ncolor of the primitive\n(B+ENTER/BACKSPASE to add/sub)", 0, 0, 0);
        // print_string(60, 150, "deleting the last primitive\nin an activated group(D)", 0, 0, 0);
        // print_string(60, 185, "deletion of an\nactivated group(DELETE)", 0, 0, 0);

    }
    void SidePane::set_child(std::shared_ptr<Widget> widget) {
        auto sp = get_position();
        auto new_pos = Coordinate {sp.x + margin.start, sp.y + margin.top};
        widget->set_position(new_pos);

        child = widget;
    }

    bool SidePane::handle_click (GLFWwindow* window, int button, int action, int mods) {
        bool capture = false;
        capture = child->handle_click(window, button, action, mods);
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
