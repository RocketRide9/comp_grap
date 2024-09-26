
#include "widget.hpp"
namespace Spark {
    void Widget::set_margin(int end, int top, int start, int bottom) {
        this->margin_end = end;
        this->margin_top = top;
        this->margin_start = start;
        this->margin_bottom = bottom;
    }
    void Widget::set_position(Coordinate coord) {
        content_bounds.place_at(coord.x + margin_start, coord.y + margin_top);
    }
    Coordinate Widget::get_position() {
        return Coordinate {
            .x = content_bounds.x1 - margin_start,
            .y = content_bounds.y1 - margin_top,
        };
    }
    int Widget::get_height() {
        return margin_top + content_bounds.get_height() + margin_bottom;
    }
    int Widget::get_width() {
        return margin_start + content_bounds.get_width() + margin_end;
    }
}
