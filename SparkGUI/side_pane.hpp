#pragma once
#include "widget.hpp"

namespace Spark {
    enum DockSide {
        START,
        TOP,
        END,
        BOTTOM,
    };
    class SidePane final : public Widget {
        public:
        SidePane(DockSide side, int size);
        void render() override;
        void set_child(Widget *widget);
        bool handle_click (GLFWwindow* window, int button, int action, int mods) override;
        // void print_edit_mode(bool is_edit_mode);

        private:
        Widget * child;
        DockSide dock_side = START;
    };
}
