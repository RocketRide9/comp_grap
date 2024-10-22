#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <chrono>
#include <iostream>
#include <ratio>
#include <vector>
#include <functional>
#include <unordered_map>
#include "spark_core.hpp"
#include <thread>

using namespace std;

namespace Spark {
    const unsigned int TARGET_FPS = 60;
    const chrono::milliseconds FRAME_TIME(1000/TARGET_FPS);
    chrono::time_point<chrono::steady_clock> last_tick;

    vector<MouseButtonCallbackFunc> mouse_button_callbacks;
    unordered_map<int, LoopFunc> loop_funcs = {};
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
        std::this_thread::sleep_for(FRAME_TIME);
        bool cont = true;
        // int to_erase = -1;
        for (auto it = loop_funcs.begin(); it != loop_funcs.end();) {
            cont = it->second(last_tick);
            if (cont) {
                it++;
            } else {
                it = loop_funcs.erase(it);
            }
        }
        last_tick = chrono::steady_clock::now();
    }
    void init(GLFWwindow* win) {
        main_window = win;
    }

    int loop_add(LoopFunc func) {
        int res = loop_funcs.size();
        loop_funcs[res] = func;
        std::cout << "added to loop a function with id: " << res << endl;

        return res;
    }
    void loop_remove(int id) {
        loop_funcs.erase(id);
        std::cout << "removed from loop a function with id: " << id << endl;
    }

    unsigned int get_frame_time() {
        return FRAME_TIME.count();
    }

    void add_mouse_callback(MouseButtonCallbackFunc func) {
        glfwSetMouseButtonCallback(main_window, NULL);
        mouse_button_callbacks.push_back(func);
        glfwSetMouseButtonCallback(main_window, main_mouse_button_callback);
    }
}  // namespace Spark
