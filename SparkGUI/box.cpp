#include "box.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>
#include "rect.hpp"
#include "widget.hpp"

namespace Spark {
    void Box::render() {
        for (auto w : children) {
            w->render();
        }
    }

    std::shared_ptr<Box> Box::create(BoxSchema schema) {
        auto res = std::shared_ptr<Box>(new Box);

        if (schema.bind) {
            *schema.bind = std::shared_ptr<Box>(res);
        }

        Rect r = Rect();
        // HACK: по какой-то причине, в некоторых случаях,
        // прямоугольник создаётся с мусором в его полях.
        // как на зло, перед отправкой в гит эта проблема
        // не появилась.
        // r.x1 = 0;
        // r.x2 = 0;
        // r.y1 = 0;
        // r.y2 = 0;
        res->content_bounds = r;
        // std::cout << r.get_width();
        // exit(1);

        res->orientation = schema.orientation;
        res->spacing = schema.spacing;
        res->margin = schema.margin;
        res->children = schema.children;

        return res;
    }

    void Box::add_child(std::shared_ptr<Widget> widget) {
        _place_child(widget);

        children.push_back(std::shared_ptr<Widget>(widget));
    }

    bool Box::handle_click(GLFWwindow* window, int button, int action, int mods) {
        bool capture = false;

        for (auto child : children) {
            capture |= child->handle_click(window, button, action, mods);
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

    void Box::_place_child (std::shared_ptr<Widget> widget) {
        auto sp = get_position();
        Coordinate new_pos;

        switch (this->orientation) {
            case VERTICAL: {
                new_pos = Coordinate {
                    sp.x + margin.start,
                    sp.y + margin.top + children_size
                };
                widget->set_position(new_pos);

                int size = content_bounds.get_width();
                int wid_size = widget->get_width();
                if (wid_size > size) {
                    content_bounds = Rect(
                        content_bounds.x1, content_bounds.y1,
                        wid_size, content_bounds.get_height()
                    );
                }

                children_size += widget->get_height() + spacing;
                break;
            }
            case HORIZONTAL: {
                new_pos = Coordinate {
                    sp.x + margin.start + children_size,
                    sp.y + margin.top
                };
                widget->set_position(new_pos);

                int size = content_bounds.get_height();
                int wid_size = widget->get_height();
                if (wid_size > size) {
                    content_bounds = Rect(
                        content_bounds.x1, content_bounds.y1,
                        content_bounds.get_width(), wid_size
                    );
                }

                children_size += widget->get_width() + spacing;
                break;
            }
        }
    }

    void Box::set_position (Coordinate coord) {
        content_bounds.place_at(coord.x + margin.start, coord.y + margin.top);
        for (auto c : children) {
            _place_child(c);
        }
    }

    Orientation Box::get_orientation() {
        return this->orientation;
    }
    void Box::set_orientation(Orientation orientation) {
        this->orientation = orientation;
        // TODO: изменить расположение дочерних виджетов
        exit(1); // не допускать вызов этой функции
    }
}
