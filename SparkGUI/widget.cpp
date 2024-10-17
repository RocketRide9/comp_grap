
#include "widget.hpp"
namespace Spark {
    void Widget::set_margin(Margin margin) {
        this->margin = margin;
    }
    void Widget::set_position(Coordinate coord) {
        content_bounds.place_at(coord.x + margin.start, coord.y + margin.top);
    }
    Coordinate Widget::get_position() {
        return Coordinate {
            .x = content_bounds.x1 - margin.start,
            .y = content_bounds.y1 - margin.top,
        };
    }
    int Widget::get_height() {
        return margin.top + content_bounds.get_height() + margin.bottom;
    }
    int Widget::get_width() {
        return margin.start + content_bounds.get_width() + margin.end;
    }
    bool Widget::handle_click (GLFWwindow* window, int button, int action, int mods) {
        return false;
    }
}
