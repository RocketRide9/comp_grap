#pragma once
#include "widget.hpp"
#include <functional>

namespace Spark {
    class Button final : public Widget {
        public:
        typedef std::function<void(Button*)> clicked_callback_func;

        Button(int width, int height);
        void clicked_connect(clicked_callback_func func);
        void render() override;

        private:
        clicked_callback_func clicked_callback;

        void register_callbacks() override;
    };
}
