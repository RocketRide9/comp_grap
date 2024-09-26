#include <iostream>
#include "rect.hpp"
#include "slider.hpp"
#include "spark_core.hpp"

using namespace std;

namespace Spark {
    Slider::Slider(int width, int height) {
        content_bounds = Rect(0, 0, width, height);
        register_callbacks();
    }
    void Slider::clicked_connect(clicked_callback_func func) {
        clicked_callback = func;
    }
    double Slider::get_value() {
        assert(value <= 1 and value >= 0);
        return value;
    }
    void Slider::render() {
        int win_width, win_height;
        auto main_window = get_main_window();
        glfwGetWindowSize(main_window, &win_width, &win_height);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0.0, win_width, win_height, 0, 0, 1);

        int width, height;
        width = content_bounds.get_width();
        height = content_bounds.get_height();

        glColor3f(value, 0, 0);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(content_bounds.x1,               content_bounds.y1);
        glVertex2f(content_bounds.x1 + width*value, content_bounds.y1);
        glVertex2f(content_bounds.x1 + width*value, content_bounds.y2);
        glVertex2f(content_bounds.x1,               content_bounds.y2);
        glEnd();

        glColor3f(0, 0, 0);
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
    void Slider::register_callbacks() {
        MouseButtonCallbackFunc f = [this](GLFWwindow* window, int button,
                                           int action, int mods) {
            if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
                double _x, _y;
                glfwGetCursorPos(window, &_x, &_y);
                cout << _x << " " << _y << endl;

                if (content_bounds.contains(_x, _y)) {
                    double value_new = (_x - content_bounds.x1) / content_bounds.get_width();
                    this->value = value_new;
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
