#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <iostream>
#include "button.hpp"
#include "rect.hpp"
#include "spark_core.hpp"

namespace Spark {
    Button::Button(int width, int height) {
        content_bounds = Rect(0, 0, width, height);
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

        glColor3f(0.25, 0.25, 0.25);
        glLineWidth(4);
        glBegin(GL_QUADS);
        glVertex2f(content_bounds.x1, content_bounds.y1);
        glVertex2f(content_bounds.x2, content_bounds.y1);
        glVertex2f(content_bounds.x2, content_bounds.y2);
        glVertex2f(content_bounds.x1, content_bounds.y2);
        glEnd();

        glBegin(GL_POINTS);
        glColor3f(0.75, 0.75, 0.75);
        glPointSize(4);
        glVertex2f(content_bounds.x1, content_bounds.y1);
        glVertex2f(content_bounds.x2, content_bounds.y1);
        glVertex2f(content_bounds.x2, content_bounds.y2);
        glVertex2f(content_bounds.x1, content_bounds.y2);
        glEnd();

        glPopMatrix();
    }
    bool Button::handle_click (GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            double _x, _y;
            glfwGetCursorPos(window, &_x, &_y);

            if (content_bounds.contains(_x, _y)) {
                std::cout << "Handling click at: " << _x << " " << _y << "\n";
                if (clicked_callback != NULL) {
                    clicked_callback(this);
                }
                return true;
            }
        }

        return false;
    }
}
