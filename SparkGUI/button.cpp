#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <cstdlib>
#include <iostream>
#include <memory>
#include "button.hpp"
#include "label.hpp"
#include "rect.hpp"
#include "spark_core.hpp"

namespace Spark {
    std::shared_ptr<Button> Button::create(ButtonSchema schema) {
        auto res = std::shared_ptr<Button>(new Button);

        if (schema.bind) {
            *schema.bind = std::shared_ptr<Button>(res);
        }
        res->margin = schema.margin;
        res->content_bounds = Rect(0, 0, schema.width, schema.height);
        res->text = schema.text;
        res->clicked_callback = schema.clicked_callback;
        res->label = Spark::Label::create({
            .width = 20,
            .height = 10,
            .text_color = { 1, 1, 1, 1 },
            .text = res->text,
        });

        return res;
    }

    void Button::clicked_connect(clicked_callback_func func) {
        clicked_callback = func;
    }

    void Button::set_position(Coordinate coord) {
        content_bounds.place_at(coord.x + margin.start, coord.y + margin.top);

        // Разместить текст по середине кнопки
        int x_pos = (content_bounds.get_width() - label->get_width()) / 2;
        int y_pos = (content_bounds.get_height() - label->get_height()) / 2;

        if (x_pos < 0 || y_pos < 0) {
            std::cerr << "WARNING: Tried to fit large label into small button\n";
        }

        label->set_position({
            content_bounds.x1 + x_pos,
            content_bounds.y1 + y_pos
        });
    }

    void Button::render() {
        int width, height;
        auto main_window = get_main_window();
        glfwGetWindowSize(main_window, &width, &height);
        glMatrixMode(GL_MODELVIEW);
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

        label->render();
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
