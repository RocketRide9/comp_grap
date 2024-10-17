#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <vector>
#include "SparkGUI/button.hpp"
#include "SparkGUI/label.hpp"
#include "SparkGUI/orientable.hpp"
#include "SparkGUI/side_pane.hpp"
#include "SparkGUI/slider.hpp"
#include "SparkGUI/spark_gui.hpp"

using namespace std;
shared_ptr<Spark::Slider> rslider;
shared_ptr<Spark::Slider> gslider;
shared_ptr<Spark::Slider> bslider;

GLint Width = 1024, Height = 512;
Spark::Rect drawing_bounds;

int points_count = 0;

struct GLPoint {
    GLfloat x, y;
};
struct Color {
    GLfloat r, g, b;
};
struct Dot {
    GLPoint point;
    Color color;
};
struct GLTriangle {
    GLPoint v1, v2, v3;
};
struct ColoredGroup {
    vector<GLTriangle> triangles;
    Color color;
};

vector <GLPoint> Points;

vector <ColoredGroup> colored_groups = {};
unsigned int active_group = 0;
bool is_edit_mode = false;
GLfloat* focused_x = NULL;
GLfloat* focused_y = NULL;

GLubyte active_mask[32][32];

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1, 1, 1, 1);
    glColor3f(0.0, 0.0, 0.0);
    glPointSize(2);

    glBegin(GL_POINTS);
    int points_size = Points.size();
    for (int i = 0; i < points_size % 3; i++) {
        int idx = points_size - 1 - i;
        glVertex2f(Points[idx].x, Points[idx].y);
    }
    glEnd();

    // Отрисовка неактивных примитивов
    glPolygonStipple(&active_mask[0][0]);
    glEnable(GL_POLYGON_STIPPLE);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < colored_groups.size(); i++) {
        if (i == active_group) {
            continue;
        }
        auto group = colored_groups[i];
        glColor3f(group.color.r, group.color.g, group.color.b);
        for (const auto triag : group.triangles) {
            glVertex2f(triag.v1.x, triag.v1.y);
            glVertex2f(triag.v2.x, triag.v2.y);
            glVertex2f(triag.v3.x, triag.v3.y);
        }
    }
    glEnd();
    glDisable(GL_POLYGON_STIPPLE);

    // Отрисовка активных примитивов
    glBegin(GL_TRIANGLES);
    auto group = colored_groups[active_group];
    glColor3f(group.color.r, group.color.g, group.color.b);
    for (const auto triag : group.triangles) {
        glVertex2f(triag.v1.x, triag.v1.y);
        glVertex2f(triag.v2.x, triag.v2.y);
        glVertex2f(triag.v3.x, triag.v3.y);
    }
    glEnd();

    if (is_edit_mode and focused_x != NULL and focused_y != NULL) {
        glPointSize(5);
        glBegin(GL_POINTS);
        glColor3f(0.7, 0., 0.7);
        glVertex2f(*focused_x, *focused_y);
        glEnd();
    }
}

bool Mouse(GLFWwindow* window, int button, int action, int mods) {
    if (is_edit_mode) {
        return false;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double _x, _y;
        glfwGetCursorPos(window, &_x, &_y);
        std::cout << "Handling click at: " << _x << " " << _y << "\n";
        _y = Height - _y;
        GLPoint p;
        p.x = _x / Width * 2 - 1;
        p.y = _y / Height * 2 - 1;

        Points.push_back(p);
        points_count++;

        if (points_count % 3 == 0) {
            GLTriangle t;
            t.v1 = Points[points_count - 1];
            t.v2 = Points[points_count - 2];
            t.v3 = Points[points_count - 3];
            // Triangles.push_back(t);
            (*(colored_groups.end() - 1)).triangles.push_back(t);
        }
    }
    return false;
}

void next_group_clicked(Spark::Button *btn) {
    auto group = colored_groups[active_group];
    auto color = group.color;
    cout << "The green button is pressed" << endl;
    colored_groups.push_back({ {}, color });
    active_group++;
}

void rslider_changed(Spark::Slider *slider) {
    auto component = slider->get_value();
    // cout << "Red value: " << component << endl;
    colored_groups[active_group].color.r = component;
}
void gslider_changed(Spark::Slider *slider) {
    auto component = slider->get_value();
    // cout << "Green value: " << component << endl;
    colored_groups[active_group].color.g = component;
}
void bslider_changed(Spark::Slider *slider) {
    auto component = slider->get_value();
    // cout << "Blue value: " << component << endl;
    colored_groups[active_group].color.b = component;
}

void rm_primitive_clicked(Spark::Button *btn) {
    auto& triangles = colored_groups[active_group].triangles;
    if (triangles.size() > 0) {
        colored_groups[active_group].triangles.pop_back();
    }
}
void rm_group_clicked(Spark::Button *btn) {
    if (active_group > 0) {
        colored_groups.pop_back();
        active_group--;
    }
    else {
        colored_groups[0].triangles.clear();
    }
}
int edit_click_id = -1;
bool edit_click_func(GLFWwindow* window, int button, int action, int mods) {
    if (!is_edit_mode) {
        return false;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double _x, _y;
        glfwGetCursorPos(window, &_x, &_y);
        if (drawing_bounds.contains(_x, _y) && focused_x != NULL && focused_y != NULL) {
            auto drag_update = [window] {
                double _x, _y;
                glfwGetCursorPos(window, &_x, &_y);
                _x = clamp(_x, 1. * drawing_bounds.x1, 1. * drawing_bounds.x2);
                _y = clamp(Height - _y, 1. * drawing_bounds.y1, 1. * drawing_bounds.y2);
                _x = _x / Width * 2 - 1;
                _y = _y / Height * 2 - 1;
                cout << "Applying new coordinate x to point: " << *focused_x << " -> " << _x << endl;
                cout << "Applying new coordinate y to point: " << *focused_y << " -> " << _y << endl;
                *focused_x = _x;
                *focused_y = _y;
                };
            edit_click_id = Spark::loop_add(drag_update);
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        if (edit_click_id != -1) {
            Spark::loop_remove(edit_click_id);
            edit_click_id = -1;
        }
    }
    return false;
}
int edit_loop_id = -1;
void edit_loop_func() {

    double cx, cy;
    GLFWwindow* window = Spark::get_main_window();
    glfwGetCursorPos(window, &cx, &cy);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) {
        // cy = Height - cy;
        // cx = cx / Width * 2 - 1;
        // cy = cy / Height * 2 - 1;
        // найти близжайшую, находящуюся в радиусе 100 пикселей
        // (в системе отсчёта -1 1, -1 1),
        // вершину к курсору и запомнить треугольник, к которой
        // она принадлежит
        GLTriangle* triag = NULL;
        GLfloat* vertex_x = NULL;
        GLfloat* vertex_y = NULL;
        GLfloat min_distance = 9999999;
        for (auto& t : colored_groups[active_group].triangles) {
            int vx1 = (t.v1.x + 1.) / 2. * Width;
            int vy1 = (-t.v1.y + 1.) / 2. * Height;
            int vx2 = (t.v2.x + 1.) / 2. * Width;
            int vy2 = (-t.v2.y + 1.) / 2. * Height;
            int vx3 = (t.v3.x + 1.) / 2. * Width;
            int vy3 = (-t.v3.y + 1.) / 2. * Height;
            // cout << "Distance: " << distance << endl;
            // без взятия корня для скорости
            GLfloat distance = pow(vx1 - cx, 2) + pow(vy1 - cy, 2);
            if (distance < min_distance && distance < 10000) {
                min_distance = distance;
                vertex_x = &t.v1.x;
                vertex_y = &t.v1.y;
            }

            distance = pow(vx2 - cx, 2) + pow(vy2 - cy, 2);
            if (distance < min_distance && distance < 10000) {
                min_distance = distance;
                vertex_x = &t.v2.x;
                vertex_y = &t.v2.y;
            }

            distance = pow(vx3 - cx, 2) + pow(vy3 - cy, 2);
            if (distance < min_distance && distance < 10000) {
                min_distance = distance;
                vertex_x = &t.v3.x;
                vertex_y = &t.v3.y;
            }
        }
        if (focused_x != NULL) {
            cout << "Found some dot nearby" << endl;
        }

        focused_x = vertex_x;
        focused_y = vertex_y;
    }
}
void edit_mode_clicked(Spark::Button *btn) {
    is_edit_mode = !is_edit_mode;
    cout << "Is edit mode?: " << is_edit_mode << endl;
    if (is_edit_mode) {
        edit_loop_id = Spark::loop_add(edit_loop_func);
    }
    else {
        Spark::loop_remove(edit_loop_id);
        edit_loop_id = -1;
    }
}

void key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        auto group = colored_groups[active_group];
        auto color = group.color;
        cout << "The green button is pressed" << endl;
        colored_groups.push_back({ {}, color });
        active_group++;
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        auto component = rslider->get_value() + 0.05;
        colored_groups[active_group].color.r = component;
        rslider->add_value(component);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
        auto component = rslider->get_value() - 0.05;
        colored_groups[active_group].color.r = component;
        rslider->add_value(component);
    }

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        auto component = gslider->get_value() + 0.05;
        colored_groups[active_group].color.g = component;
        gslider->add_value(component);
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
        auto component = gslider->get_value() - 0.05;
        colored_groups[active_group].color.g = component;
        gslider->add_value(component);
    }

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        auto component = bslider->get_value() + 0.05;
        colored_groups[active_group].color.b = component;
        bslider->add_value(component);
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) {
        auto component = bslider->get_value() - 0.05;
        colored_groups[active_group].color.b = component;
        bslider->add_value(component);
    }

    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        auto& triangles = colored_groups[active_group].triangles;
        if (triangles.size() > 0) {
            colored_groups[active_group].triangles.pop_back();
        }
    }
    if (key == GLFW_KEY_DELETE && action == GLFW_PRESS) {
        if (active_group > 0) {
            colored_groups.pop_back();
            active_group--;
        }
        else {
            colored_groups[0].triangles.clear();
        }
    }
    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        is_edit_mode = !is_edit_mode;
        cout << "Is edit mode?: " << is_edit_mode << endl;
        if (is_edit_mode) {
            edit_loop_id = Spark::loop_add(edit_loop_func);
        }
        else {
            Spark::loop_remove(edit_loop_id);
            edit_loop_id = -1;
        }
    }
}

int main(void) {
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(Width, Height, "Triangle", NULL, NULL);
    Spark::init(window);

    if (!window) {
        glfwTerminate();
        return -1;
    }
    colored_groups.push_back({});

    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 32; x++) {
            active_mask[y][x] = (y - x) % 2;
        }
    }

    // UI
    glfwMakeContextCurrent(window);
    drawing_bounds = Spark::Rect(480, 0, Width - 480, Height);

    // Spark::Rect r;
    // std::cout << r.get_width();
    // exit(1);

    using namespace Spark;
    auto main_pane = SidePane::create({
        .side = START,
        .size = 480,
        .child = Box::create({
            .margin = { .top = 10, .start = 10 },
            .orientation = VERTICAL,
            .spacing = 30,
            .children = {
                Box::create({
                    .orientation = HORIZONTAL,
                    .spacing = 10,
                    .children = {
                        Button::create({
                            .width =  100, .height = 40,
                            .clicked_callback = next_group_clicked,
                        }),
                        Label::create({
                            .width = 100, .height = 20,
                            .text = "add a new group of\nprimitives(N)"
                        })
                    }
                }),
                Box::create({
                    .orientation = HORIZONTAL,
                    .spacing = 10,
                    .children = {
                        Slider::create({
                            .width =  100, .height = 40,
                            .changed_callback = rslider_changed,
                        }),
                        Label::create({
                            .width = 100, .height = 20,
                            .text = "changing the red component\ncolor of the primitive\n(R+ENTER/BACKSPASE to add/sub)"
                        })
                    }
                }),
                Box::create({
                    .orientation = HORIZONTAL,
                    .spacing = 10,
                    .children = {
                        Slider::create({
                            .width = 100, .height = 40,
                            .changed_callback = gslider_changed,
                        }),
                        Label::create({
                            .width = 100, .height = 20,
                            .text = "changing the green component\ncolor of the primitive\n(G+ENTER/BACKSPASE to add/sub)"
                        })
                    }
                }),
                Box::create({
                    .orientation = HORIZONTAL,
                    .spacing = 10,
                    .children = {
                        Slider::create({
                            .width =  100, .height = 40,
                            .changed_callback = bslider_changed,
                        }),
                        Label::create({
                            .width = 100, .height = 20,
                            .text = "changing the green component\ncolor of the primitive\n(G+ENTER/BACKSPASE to add/sub)"
                        })
                    }
                }),
                Box::create({
                    .orientation = HORIZONTAL,
                    .spacing = 10,
                    .children = {
                        Button::create({
                            .width =  100, .height = 40,
                            .clicked_callback = rm_group_clicked,
                        }),
                        Label::create({
                            .width = 100, .height = 20,
                            .text = "deleting the last primitive\nin an activated group(D)"
                        })
                    }
                }),
                Box::create({
                    .orientation = HORIZONTAL,
                    .spacing = 10,
                    .children = {
                        Button::create({
                            .width =  100, .height = 40,
                            .clicked_callback = rm_primitive_clicked,
                        }),
                        Label::create({
                            .width = 100, .height = 20,
                            .text = "deletion of an\nactivated group(DELETE)"
                        })
                    }
                }),
                Box::create({
                    .orientation = HORIZONTAL,
                    .spacing = 10,
                    .children = {
                        Button::create({
                            .width =  100, .height = 40,
                            .clicked_callback = edit_mode_clicked,
                        }),
                        Label::create({
                            .width = 100, .height = 20,
                            .text = "edit coordinates mode\nfor primitives: enabled\n(press 1 to switch mode)"
                        })
                    }
                }),
            }
         })
    });

    Spark::add_mouse_callback(edit_click_func);
    Spark::add_mouse_callback(Mouse);

    glfwSetKeyCallback(window, key);
    // UI end

    while (!glfwWindowShouldClose(window))
    {
        Spark::loop_iterate();
        display();
        main_pane->render();

        glfwSwapBuffers(window);
        glfwWaitEvents();
    }

    glfwTerminate();
    return 0;
}
