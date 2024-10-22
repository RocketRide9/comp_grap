#pragma once
#if !defined (SPARK_INSIDE) and !defined (SPARK_COMPILATION)
    #error "This header can't be included directly. Please include "SparkGUI/spark.hpp" instead."
#endif

#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <cassert>
#include <functional>
#include <chrono>

namespace Spark {
    template <typename T>
    struct RequiredField {
        RequiredField(const T& value) : value(value) {}
        operator T&() { return value; }
        operator const T&() const { return value; }
        T value;
    };

    typedef std::function<bool (GLFWwindow* window, int button, int action, int mods)>
        MouseButtonCallbackFunc;

    // функция, помещаемая в цикл
    // возвращает False, если эта функция должна быть убрана из цикла
    typedef std::function<bool(std::chrono::time_point<std::chrono::steady_clock>&)>
        LoopFunc;

    GLFWwindow* get_main_window();
    void init(GLFWwindow* win);
    void loop_iterate();
    int loop_add(LoopFunc func);
    void loop_remove(int id);
    void add_mouse_callback(MouseButtonCallbackFunc func);
    unsigned int get_frame_time();
} // Spark
