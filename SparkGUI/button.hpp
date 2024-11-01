#pragma once
#if !defined (SPARK_INSIDE) and !defined (SPARK_COMPILATION)
    // #error "This header can't be included directly. Please include "SparkGUI/spark.hpp" instead."
#endif

#include <functional>
#include <memory>
#include "widget.hpp"
#include "label.hpp"
#include "spark.hpp"

namespace Spark {
    class Button;
    typedef std::function<void(Button *)> clicked_callback_func;

    struct ButtonSchema {
        std::shared_ptr<Button> * bind = nullptr;

        Margin margin = {};
        std::string text = "";

        RequiredField<int> width;
        RequiredField<int> height;
        clicked_callback_func clicked_callback = nullptr;
    };

    class Button final : public Widget {
        public:
        static std::shared_ptr<Button> create(ButtonSchema schema);

        void clicked_connect(clicked_callback_func func);
        void render() override;
        void set_position(Coordinate coord) override;
        bool handle_click (GLFWwindow* window, int button, int action, int mods) override;

        private:
        Button() {};
        std::shared_ptr<Spark::Label> label;
        std::string text;
        clicked_callback_func clicked_callback;
    };
}
