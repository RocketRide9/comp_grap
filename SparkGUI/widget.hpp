#pragma once
#if !defined (SPARK_INSIDE) and !defined (SPARK_COMPILATION)
    #error "This header can't be included directly. Please include "SparkGUI/spark.hpp" instead."
#endif

#include <vector>
#include "rect.hpp"
#include "spark_core.hpp"

namespace Spark {
    // TODO: поле выделенной области?
    // оно пригодилось бы для виджетов, которые
    // хотят располагаться посередине или в конце некоторой
    // выделенной области
    struct Margin {
        int end = 0;
        int top = 0;
        int start = 0;
        int bottom = 0;
    };

    class Widget {
        public:
        virtual Coordinate get_position();
        virtual int get_height();
        virtual int get_width();
        virtual void set_position(Coordinate coord);
        virtual void set_margin(Margin margin);
        virtual bool handle_click (GLFWwindow* window, int button, int action, int mods);
        virtual void render() = 0;

        protected:
        Margin margin;
        Rect content_bounds = {};
        // только само содержимое виджета, не включает в себя отступы
        std::vector<MouseButtonCallbackFunc> mouse_callbacks;
    };
}
