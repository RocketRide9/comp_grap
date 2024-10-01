#pragma once
#include <functional>
#include "widget.hpp"

namespace Spark {
    class Button final : public Widget {
        public:
        typedef std::function<void(Button*)> clicked_callback_func;

        Button(int width, int height);
        void clicked_connect(clicked_callback_func func);
        void render() override;
        bool handle_click (GLFWwindow* window, int button, int action, int mods) override;

        private:
        clicked_callback_func clicked_callback;
    };
}
