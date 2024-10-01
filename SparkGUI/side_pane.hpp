#pragma once
#include "widget.hpp"
#include "orientable.hpp"
#include <vector>

namespace Spark {
    enum DockSide {
        START,
        TOP,
        END,
        BOTTOM,
    };
    class SidePane final : public Widget {
        public:
        SidePane(DockSide side, int size, int padding);
        void render() override;
        void add_child(Widget *widget);
        bool handle_click (GLFWwindow* window, int button, int action, int mods) override;
        void print_edit_mode(bool is_edit_mode);

        private:
        std::vector<Widget *> children;
        int children_height = 0;
        int children_width = 0;
        DockSide dock_side = START;
        Orientation orientation = VERTICAL;
        int padding;

    };
}
