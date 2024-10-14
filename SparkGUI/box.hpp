#pragma once
#include "widget.hpp"
#include "orientable.hpp"
#include <vector>

namespace Spark {
    // виджет для представления нескольких виджетов в ряд
    // его размеры зависят от размеров дочерних виджетов
    class Box final : public Widget, public Orientable {
        public:
        Box(Orientation orientation, int padding);
        void render() override;
        void add_child(Widget *widget);
        bool handle_click (GLFWwindow* window, int button, int action, int mods) override;

        void set_orientation(Orientation orientation) override;
        Orientation get_orientation() override;

        private:
        std::vector<Widget *> children;
        // размер доч. виджетов.
        // высота или ширина в зависимости от направления коробки
        int children_size = 0;
        Orientation orientation = VERTICAL;
        int padding;
    };
}
