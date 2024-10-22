#pragma once
#if !defined (SPARK_INSIDE) and !defined (SPARK_COMPILATION)
    #error "This header can't be included directly. Please include "SparkGUI/spark.hpp" instead."
#endif

#include "widget.hpp"
#include <functional>
#include <memory>

namespace Spark {
    class Slider;
    typedef std::function<void(Slider *)> changed_callback_func;

    struct SliderSchema {
        std::shared_ptr<Slider> * bind = nullptr;

        Margin margin = {};

        RequiredField<int> width;
        RequiredField<int> height;
        changed_callback_func changed_callback = nullptr;
    };

    class Slider final : public Widget {
        public:
        static std::shared_ptr<Slider> create(SliderSchema schema);

        void changed_connect(changed_callback_func func);
        void add_value(double value);
        double get_value();
        void render() override;
        // void set_content_size(int width, int height) override;
        bool handle_click (GLFWwindow* window, int button, int action, int mods) override;

        private:
        Slider() {}
        /* Текущее значение слайдера от 0 до 1 */
        double value = 0;
        changed_callback_func changed_callback;
        int loop_func_id = -1;
    };
}
