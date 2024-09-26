#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "spark_core.hpp"
#include <vector>
#include <functional>
#include <unordered_map>

using namespace std;

namespace Spark {
    vector<MouseButtonCallbackFunc> mouse_button_callbacks;
    unordered_map<int, function<void()>> loop_funcs = {};
    GLFWwindow* main_window;
    GLFWwindow* get_main_window() {
        return main_window;
    }
    void main_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        for (auto a : mouse_button_callbacks) {
            bool capture = a(window, button, action, mods);
            if (capture) {
                return;
            }
        }
    }
    void loop_iterate() {
        for (auto a : loop_funcs) {
            a.second();
        }
    }
    void init(GLFWwindow* win) {
        main_window = win;
    }

    int loop_add(function<void()> func) {
        int res = loop_funcs.size();
        loop_funcs[res] = func;

        return res;
    }
    void loop_remove(int id) {
        loop_funcs.erase(id);
    }

    void add_mouse_callback(MouseButtonCallbackFunc func) {
        glfwSetMouseButtonCallback(main_window, NULL);
        mouse_button_callbacks.push_back(func);
        glfwSetMouseButtonCallback(main_window, main_mouse_button_callback);
    }
}  // namespace Spark
