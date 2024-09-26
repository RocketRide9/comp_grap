#include "side_pane.hpp"
#include "rect.hpp"
#include "spark_core.hpp"
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
        register_callbacks();
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

        std::cout << "child widget height = " << widget->get_height() << std::endl;

        children_height += widget->get_height() + padding;
        std::cout << "childrens height = " << children_height << std::endl;

        children.push_back(widget);
    }

    // Private

    void SidePane::register_callbacks() {}
}
