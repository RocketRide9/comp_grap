#pragma once
#include <vector>
#include "rect.hpp"
#include "spark_core.hpp"

namespace Spark {
    // TODO: поле выделенной области?
    // оно пригодилось бы для виджетов, которые
    // хотят располагаться посередине или в конце некоторой
    // выделенной области
    class Widget {
        public:
        virtual Coordinate get_position();
        virtual int get_height();
        virtual int get_width();
        virtual void set_position(Coordinate coord);
        virtual void set_margin(int end, int top, int start, int bottom);
        virtual bool handle_click (GLFWwindow* window, int button, int action, int mods);
        virtual void render() = 0;
        protected:
        int margin_end = 0;
        int margin_top = 0;
        int margin_start = 0;
        int margin_bottom = 0;
        Rect content_bounds; // только само содержимое виджета, не включает в себя отступы
        std::vector<MouseButtonCallbackFunc> mouse_callbacks;
    };
}
