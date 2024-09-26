#pragma once
#include "rect.hpp"

namespace Spark {
    class Widget {
        public:
        virtual void render() = 0;
        virtual void set_position(Coordinate coord);
        virtual Coordinate get_position();
        virtual int get_height();
        virtual int get_width();
        // virtual void set_content_size(int width, int height) = 0;
        // virtual Rect get_bounds() = 0;
        // virtual void set_bounds(Rect &bounds) = 0;
        virtual void set_margin(int end, int top, int start, int bottom);
        protected:
        int margin_end = 0;
        int margin_top = 0;
        int margin_start = 0;
        int margin_bottom = 0;
        Rect content_bounds;
        virtual void register_callbacks() = 0;
    };
}
