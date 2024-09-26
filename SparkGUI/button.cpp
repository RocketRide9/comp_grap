#include <GLFW/glfw3.h>
#include "button.hpp"
#include "rect.hpp"
#include "spark_core.hpp"

namespace Spark {
    Button::Button(int width, int height) {
        content_bounds = Rect(0, 0, width, height);
        register_callbacks();
    }
    void Button::clicked_connect(clicked_callback_func func) {
        clicked_callback = func;
    }
    void Button::render() {
        int width, height;
        auto main_window = get_main_window();
        glfwGetWindowSize(main_window, &width, &height);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0.0, width, height, 0, 0, 1);

        glColor3f(0.0, 1., 0.0);
        glLineWidth(4);
        glBegin(GL_LINE_LOOP);
        glVertex2f(content_bounds.x1, content_bounds.y1);
        glVertex2f(content_bounds.x2, content_bounds.y1);
        glVertex2f(content_bounds.x2, content_bounds.y2);
        glVertex2f(content_bounds.x1, content_bounds.y2);
        glEnd();

        glPopMatrix();
    }

    // Private
    void Button::register_callbacks() {
        MouseButtonCallbackFunc f = [this](GLFWwindow* window, int button,
                                           int action, int mods) {
            if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
                double _x, _y;
                glfwGetCursorPos(window, &_x, &_y);

                if (content_bounds.contains(_x, _y)) {
                    if (clicked_callback != NULL) {
                        clicked_callback(this);
                    }
                    return true;
                }
            }

            return false;
        };
        add_mouse_callback(f);
    }
}
