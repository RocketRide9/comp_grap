#pragma once
#include "widget.hpp"
#include <functional>

namespace Spark {
    class Slider final : public Widget {
        public:
        typedef std::function<void(Slider*)> clicked_callback_func;

        Slider(int width, int height);
        void clicked_connect(clicked_callback_func func);
        double get_value();
        void render() override;
        // void set_content_size(int width, int height) override;

        private:
        /* Текущее значение слайдера от 0 до 1 */
        double value = 0;
        clicked_callback_func clicked_callback;

        void register_callbacks() override;
    };
}
