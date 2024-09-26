#include "side_pane.hpp"
#include "rect.hpp"
#include "spark_core.hpp"
#include <functional>
#include <iostream>

namespace Spark {
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
