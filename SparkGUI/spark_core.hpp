#include <GL/gl.h>
#include <GLFW/glfw3.h>
#pragma once
#include <cassert>
#include <functional>

namespace Spark {

    typedef std::function<
        bool (GLFWwindow* window, int button, int action, int mods)
        >
        MouseButtonCallbackFunc;

    GLFWwindow* get_main_window();
    void init(GLFWwindow* win);
    void add_mouse_callback(MouseButtonCallbackFunc func);

} // Spark
