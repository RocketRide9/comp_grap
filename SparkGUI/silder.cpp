#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <algorithm>
#include <iostream>
#include "rect.hpp"
#include "side_pane.hpp"
#include "slider.hpp"
#include "spark_core.hpp"

using namespace std;

namespace Spark {
    Slider::Slider(int width, int height) {
        content_bounds = Rect(0, 0, width, height);
    }
    void Slider::clicked_connect(clicked_callback_func func) {
        clicked_callback = func;
    }
    void Slider::add_value(double _value) {    
        double EPS = 1e-15;
        if (fabs(_value) < EPS || _value < 0)
            _value = 0;
        if(fabs(_value - 1) < EPS || _value > 1)
            _value = 1;
        this->value = _value;
        assert(value <= 1 and value >= 0);
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

        glColor3ub(76, 175, 80);
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
    bool Slider::handle_click (GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            double _x, _y;
            glfwGetCursorPos(window, &_x, &_y);
            if (content_bounds.contains(_x, _y)) {
                if (clicked_callback != NULL) {
                    clicked_callback(this);
                }

                auto slider_update = [this, window]{
                    double _x, _y;
                    glfwGetCursorPos(window, &_x, &_y);
                    int x = _x;
                    x = clamp(x, content_bounds.x1, content_bounds.x2);

                    double value_new = 1. * (x-content_bounds.x1) / content_bounds.get_width();
                    this->value = value_new;
                    if (clicked_callback != NULL) {
                        clicked_callback(this);
                    }
                };
                loop_func_id = Spark::loop_add(slider_update);
                return true;
            }
        } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
            if (loop_func_id != -1) {
                Spark::loop_remove(loop_func_id);
                loop_func_id = -1;
            }
        }

        return false;
    }
}
