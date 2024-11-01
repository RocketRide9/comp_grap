#include "box.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>
#include "orientable.hpp"
#include "rect.hpp"
#include "widget.hpp"

namespace Spark {
    void Box::render() {
        if (bg_color.a != 0)
        {
            glColor4f(bg_color.r, bg_color.g, bg_color.b, bg_color.a);
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(content_bounds.x1, content_bounds.y1);
            glVertex2f(content_bounds.x2, content_bounds.y1);
            glVertex2f(content_bounds.x2, content_bounds.y2);
            glVertex2f(content_bounds.x1, content_bounds.y2);
            glEnd();
        }

        for (auto w : children) {
            w->render();
        }
    }

    std::shared_ptr<Box> Box::create(BoxSchema schema) {
        auto res = std::shared_ptr<Box>(new Box);

        if (schema.bind) {
            *schema.bind = std::shared_ptr<Box>(res);
        }

        Rect r = Rect(0, 0, schema.min_width, schema.min_height);
        // HACK: по какой-то причине, в некоторых случаях,
        // прямоугольник создаётся с мусором в его полях.
        // как на зло, перед отправкой в гит эта проблема
        // не появилась.
        // r.x1 = 0;
        // r.x2 = 0;
        // r.y1 = 0;
        // r.y2 = 0;
        res->content_bounds = r;
        res->bg_color = schema.bg_color;
        // std::cout << r.get_width();
        // exit(1);

        res->orientation = schema.orientation;
        res->spacing = schema.spacing;
        res->margin = schema.margin;
        res->children = schema.children;

        return res;
    }

    void Box::add_child(std::shared_ptr<Widget> widget) {
        _add_spacing();

        _place_child(widget);

        _rem_spacing();

        children.push_back(std::shared_ptr<Widget>(widget));
    }

    bool Box::handle_click(GLFWwindow* window, int button, int action, int mods) {
        // если виджет неактивен, ничего не делать и не захватывать
        // нажатие
        if (!active) {
            return false;
        }

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

                int child_height = widget->get_width();
                children_size += widget->get_height() + spacing;

                content_bounds = Rect(
                    content_bounds.x1, content_bounds.y1,
                    std::max(child_height, content_bounds.get_width()), std::max(children_size, content_bounds.get_height())
                );

                break;
            }
            case HORIZONTAL: {
                new_pos = Coordinate {
                    sp.x + margin.start + children_size,
                    sp.y + margin.top
                };
                widget->set_position(new_pos);

                int child_height = widget->get_height();
                children_size += widget->get_width() + spacing;

                content_bounds = Rect(
                    content_bounds.x1, content_bounds.y1,
                    std::max(children_size, content_bounds.get_width()), std::max(child_height, content_bounds.get_height())
                );

                break;
            }
        }
    }

    // HACK: убрать отступ за последним виджетом
    void Box::_rem_spacing()
    {
        children_size -= spacing;

        switch (this->orientation) {
            case VERTICAL:
                content_bounds.y2 -= spacing;
                break;
            case HORIZONTAL:
                content_bounds.x2 -= spacing;
                break;
        }
    }

    // HACK: добавить отступ за последним виджетом
    void Box::_add_spacing()
    {
        children_size += spacing;

        switch (this->orientation) {
            case VERTICAL:
                content_bounds.y2 += spacing;
                break;
            case HORIZONTAL:
                content_bounds.x2 += spacing;
                break;
        }
    }

    void Box::set_position (Coordinate coord) {
        children_size = 0;
        content_bounds.place_at(coord.x + margin.start, coord.y + margin.top);
        for (auto c : children) {
            _place_child(c);
        }

        _rem_spacing();
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
