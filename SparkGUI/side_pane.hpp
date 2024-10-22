#pragma once
#if !defined (SPARK_INSIDE) and !defined (SPARK_COMPILATION)
    #error "This header can't be included directly. Please include "SparkGUI/spark.hpp" instead."
#endif

#include <memory>
#include "widget.hpp"

namespace Spark {
    enum DockSide {
        START,
        TOP,
        END,
        BOTTOM,
    };
    class SidePane;

    struct SidePaneSchema {
        std::shared_ptr<SidePane> * bind = nullptr;

        RequiredField<DockSide> side;
        RequiredField<int> size;
        std::shared_ptr<Widget> child;
    };

    class SidePane final : public Widget {
        public:
        static std::shared_ptr<SidePane> create(SidePaneSchema schema);

        // SidePane(DockSide side, int size);
        void render() override;
        void set_child(std::shared_ptr<Widget> widget);
        bool handle_click (GLFWwindow* window, int button, int action, int mods) override;
        // void print_edit_mode(bool is_edit_mode);

        private:
        SidePane() {}
        std::shared_ptr<Widget> child;
        DockSide dock_side = START;
    };
}
