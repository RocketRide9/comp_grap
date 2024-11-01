
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "SparkGUI/box.hpp"
#include "SparkGUI/button.hpp"
#include "SparkGUI/label.hpp"
#include "SparkGUI/orientable.hpp"
#include "SparkGUI/spark.hpp"

using namespace std;

const int win_width = 600;
const int win_height = 400;

shared_ptr<Spark::Box> popover = nullptr;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1, 1, 1, 1);

    // glColor3f(1, 0, 1);
    // glBegin(GL_TRIANGLE_FAN);
    // glVertex2f(300, 200);
    // glVertex2f(300, 400);
    // glVertex2f(400, 400);
    // glVertex2f(400, 200);
    // glEnd();
}

bool right_click(GLFWwindow* window, int button, int action, int mods)
{
    double _x, _y;
    glfwGetCursorPos(window, &_x, &_y);
    int x = _x;
    int y = _y;

    if (popover->handle_click(window, button, action, mods)) {
        return true;
    }

    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT) {
        popover->set_position({x, y});
        popover->active = true;
        return true;
    }
    else if (action == GLFW_PRESS) {
        if (popover->active) {
            popover->active = false;
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

int main()
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(win_width, win_height, "Тест", NULL, NULL);
    Spark::init(window);

    glfwMakeContextCurrent(window);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(0.0, win_width, win_height, 0, 0, 200);

    auto pane = Spark::SidePane::create({
        .side = Spark::START,
        .size = 200,
        .child = Spark::Box::create({
            .margin = { 5, 5, 5, 5 },
            .orientation = Spark::VERTICAL,
            .spacing = 10,
            .children = {
                Spark::Label::create({
                    .width = 50,
                    .height = 30,
                    .text = "Hello World!\nNewline",
                }),
                Spark::Label::create({
                    .width = 50,
                    .height = 30,
                    .text = "Hello World!",
                })
            }
        })
    });

    popover = Spark::Box::create({
        .bg_color = {0.2, 0.6, 0.2, 1},
        .children = {
            Spark::Box::create({
                .margin = {5, 5, 5, 5},
                .spacing = 5,
                .children = {
                    Spark::Box::create({
                        .orientation = Spark::HORIZONTAL,
                        .spacing = 5,
                        .children = {
                            Spark::Label::create({
                                .width = 90,
                                .height = 30,
                                .text_color = { 1, 1, 1, 1 },
                                .text = "X scale:",
                            }),
                            Spark::Button::create({
                                .text = "+",
                                .width = 50,
                                .height = 30,
                            }),
                            Spark::Button::create({
                                .text = "-",
                                .width = 50,
                                .height = 30,
                            }),
                        }
                    }),
                    Spark::Box::create({
                        .orientation = Spark::HORIZONTAL,
                        .spacing = 5,
                        .children = {
                            Spark::Label::create({
                                .width = 90,
                                .height = 30,
                                .text_color = { 1, 1, 1, 1 },
                                .text = "Y scale:",
                            }),
                            Spark::Button::create({
                                .text = "+",
                                .width = 50,
                                .height = 30,
                            }),
                            Spark::Button::create({
                                .text = "-",
                                .width = 50,
                                .height = 30,
                            }),
                        }
                    }),
                    Spark::Box::create({
                        .orientation = Spark::HORIZONTAL,
                        .spacing = 5,
                        .children = {
                            Spark::Label::create({
                                .width = 90,
                                .height = 30,
                                .text_color = { 1, 1, 1, 1 },
                                .text = "Z scale:",
                            }),
                            Spark::Button::create({
                                .text = "+",
                                .width = 50,
                                .height = 30,
                            }),
                            Spark::Button::create({
                                .text = "-",
                                .width = 50,
                                .height = 30,
                            }),
                        }
                    }),
                    Spark::Button::create({
                        .text = "Toggle Normals",
                        .width = 200,
                        .height = 30,
                    }),
                    Spark::Button::create({
                        .text = "Toggle Antialising",
                        .width = 200,
                        .height = 30,
                    }),
                    Spark::Button::create({
                        .text = "Toggle Wires",
                        .width = 200,
                        .height = 30,
                    }),
                    Spark::Button::create({
                        .text = "Toggle Texture",
                        .width = 200,
                        .height = 30,
                    }),
                    Spark::Button::create({
                        .text = "Toggle Projection",
                        .width = 200,
                        .height = 30,
                    })
                }
            })
        }
    });
    // по умолчаению PopOver не реагирует на нажатия
    popover->active = false;

    Spark::add_mouse_callback(right_click);

    while (!glfwWindowShouldClose(window))
    {
        display();
        Spark::loop_iterate();
        pane->render();

        if (popover->active)
        {
            popover->render();
        }

        glfwSwapBuffers(window);
        glfwWaitEvents();
    }

    glfwTerminate();
    return 0;

}
