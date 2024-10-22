#pragma once
#if !defined (SPARK_INSIDE) and !defined (SPARK_COMPILATION)
    #error "This header can't be included directly. Please include "SparkGUI/spark.hpp" instead."
#endif

#include "rect.hpp"
#include "widget.hpp"
#include "orientable.hpp"
#include <memory>
#include <vector>

namespace Spark {
    class Box;

    struct BoxSchema {
        std::shared_ptr<Box> * bind = nullptr;

        Margin margin = {};
        Orientation orientation = VERTICAL;
        int spacing = 0;
        std::vector<std::shared_ptr<Widget>> children = {};
    };

    // виджет для представления нескольких виджетов в ряд
    // его размеры зависят от размеров дочерних виджетов
    class Box final : public Widget, public Orientable {
        public:
        static std::shared_ptr<Box> create(BoxSchema schema);

        void render() override;
        void add_child(std::shared_ptr<Widget> widget);
        bool handle_click (GLFWwindow* window, int button, int action, int mods) override;
        void set_position (Coordinate coord) override;

        void set_orientation(Orientation orientation) override;
        Orientation get_orientation() override;

        private:
        Box() {};
        void _place_child (std::shared_ptr<Widget> widget);

        std::vector<std::shared_ptr<Widget>> children;
        // размер доч. виджетов.
        // высота или ширина в зависимости от направления коробки
        int children_size = 0;
        Orientation orientation ;
        int spacing;
    };
}
