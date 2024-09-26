#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include "spark_core.hpp"
#include <vector>

using namespace std;

namespace Spark {
    vector<MouseButtonCallbackFunc> mouse_button_callbacks;
    GLFWwindow* main_window;
    GLFWwindow* get_main_window() {
        return main_window;
    }
    void main_mouse_button_callback (GLFWwindow* window, int button, int action, int mods) {
        for (auto a : mouse_button_callbacks) {
            bool capture = a(window, button, action, mods);
            if (capture) {
                return;
            }
        }
    }
    void init(GLFWwindow* win) {
        main_window = win;
    }

    void add_mouse_callback(MouseButtonCallbackFunc func) {
        glfwSetMouseButtonCallback(main_window, NULL);
        mouse_button_callbacks.push_back(func);
        glfwSetMouseButtonCallback(main_window, main_mouse_button_callback);
    }
}  // namespace Spark
