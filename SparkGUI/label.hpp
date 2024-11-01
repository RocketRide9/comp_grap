#pragma once
#if !defined (SPARK_INSIDE) and !defined (SPARK_COMPILATION)
    // #error "This header can't be included directly. Please include "SparkGUI/spark.hpp" instead."
#endif

#include <memory>
#include "widget.hpp"

namespace Spark {
    class Label;

    struct LabelSchema {
        std::shared_ptr<Label> * bind = nullptr;

        Margin margin = {};
        RequiredField<int> width;
        RequiredField<int> height;
        RGBA text_color = { 0, 0, 0, 1};
        std::string text = "";
    };
    // виджет, отображающий текст
    class Label: public Widget {
        public:
        static std::shared_ptr<Label> create(LabelSchema schema);

        bool handle_click (GLFWwindow* window, int button, int action, int mods) override;
        void render() override;

        void set_text(std::string &text);
        std::string get_text();

        protected:
        Label() {}
        std::string text;
        RGBA text_color;
    };
}
