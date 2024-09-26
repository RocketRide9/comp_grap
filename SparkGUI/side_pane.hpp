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

        private:
        std::vector<Widget *> children;
        int children_height = 0;
        int children_width = 0;
        DockSide dock_side = START;
        Orientation orientation = VERTICAL;
        int padding;

        void register_callbacks() override;
    };
}
