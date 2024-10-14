#include "box.hpp"
#include <iostream>

namespace Spark {
    void Box::render() {
        for (auto w : children) {
            w->render();
        }
    }

    Box::Box(Orientation orientation, int padding)
        : padding(padding), orientation(orientation) {}

    void Box::add_child(Widget *widget) {
        auto sp = get_position();
        Coordinate new_pos;

        switch (this->orientation) {
            case VERTICAL: {
                new_pos = Coordinate {
                    sp.x + margin_start,
                    sp.y + margin_top + children_size
                };

                int size = content_bounds.get_width();
                int wid_size = widget->get_width();
                if (wid_size > size) {
                    content_bounds = Rect(
                        content_bounds.x1, content_bounds.y1,
                        wid_size, content_bounds.get_height()
                    );
                }

                widget->set_position(new_pos);
                children_size += widget->get_height() + padding;
                break;
            }
            case HORIZONTAL: {
                new_pos = Coordinate {
                    sp.x + margin_start + children_size,
                    sp.y + margin_top
                };

                int size = content_bounds.get_height();
                int wid_size = widget->get_height();
                if (wid_size > size) {
                    content_bounds = Rect(
                        content_bounds.x1, content_bounds.y1,
                        content_bounds.get_width(), wid_size
                    );
                }

                std::cout << new_pos.x << " " << new_pos.y << std::endl;

                widget->set_position(new_pos);
                children_size += widget->get_width() + padding;
                break;
            }
        }

        children.push_back(widget);
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

    Orientation Box::get_orientation() {
        return this->orientation;
    }
    void Box::set_orientation(Orientation orientation) {
        this->orientation = orientation;
        // TODO: изменить расположение дочерних виджетов
        exit(1); // не допускать вызов этой функции
    }
}
