#pragma once
#include "rect.hpp"
#include "widget.hpp"

namespace Spark {
    // виджет, отображающий текст
    class Label: public Widget {
        public:
        bool handle_click (GLFWwindow* window, int button, int action, int mods) override;
        void render() override;

        Label(int width, int height, std::string text);
        void set_text(std::string &text);
        std::string get_text();

        protected:
        std::string text = "";
    };
}
