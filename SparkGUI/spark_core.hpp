#pragma once
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <cassert>
#include <functional>

namespace Spark {

    typedef std::function<bool (GLFWwindow* window, int button, int action, int mods)>
        MouseButtonCallbackFunc;

    GLFWwindow* get_main_window();
    void init(GLFWwindow* win);
    void loop_iterate();
    int loop_add(std::function<void()> func);
    void loop_remove(int id);
    void add_mouse_callback(MouseButtonCallbackFunc func);

} // Spark
