#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include "SparkGUI/rect.hpp"
#include "SparkGUI/spark_core.hpp"
#include "SparkGUI/spark_gui.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <vector>

using namespace std;

GLint Width = 512, Height = 512;
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
GLfloat *focused_x = NULL;
GLfloat *focused_y = NULL;

GLubyte active_mask[32][32];

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1, 1, 1, 1);
    glColor3f(0.0, 0.0, 0.0);
    glPointSize(2);

    glBegin(GL_POINTS);
    int points_size = Points.size();
    for (int i = 0; i < points_size%3; i++) {
        int idx = points_size-1-i;
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
            (*(colored_groups.end()-1)).triangles.push_back(t);
        }
    }
    return false;
}

void next_group_clicked (Spark::Button *btn) {
    auto group = colored_groups[active_group];
    auto color = group.color;
    cout << "Нажата зелёная кнопка" << endl;
    colored_groups.push_back({{}, color});
    active_group++;
}

void rslider_changed (Spark::Slider *slider) {
    auto component = slider->get_value();
    // cout << "Red value: " << component << endl;
    colored_groups[active_group].color.r =  component;
}
void gslider_changed (Spark::Slider *slider) {
    auto component = slider->get_value();
    // cout << "Green value: " << component << endl;
    colored_groups[active_group].color.g =  component;
}
void bslider_changed (Spark::Slider *slider) {
    auto component = slider->get_value();
    // cout << "Blue value: " << component << endl;
    colored_groups[active_group].color.b =  component;
}

void rm_primitive_clicked (Spark::Button *btn) {
    auto &triangles = colored_groups[active_group].triangles;
    if (triangles.size() > 0) {
        colored_groups[active_group].triangles.pop_back();
    }
}
void rm_group_clicked (Spark::Button *btn) {
    if (active_group > 0) {
        colored_groups.pop_back();
        active_group--;
    } else {
        colored_groups[0].triangles.clear();
    }
}
int edit_click_id = -1;
bool edit_click_func (GLFWwindow* window, int button, int action, int mods) {
    if (!is_edit_mode) {
        return false;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double _x, _y;
        glfwGetCursorPos(window, &_x, &_y);
        if (drawing_bounds.contains(_x, _y)) {
            auto drag_update = [window] {
                double _x, _y;
                glfwGetCursorPos(window, &_x, &_y);
                _x = clamp(_x, 1.*drawing_bounds.x1, 1.*drawing_bounds.x2);
                _y = Height - _y;
                _x = _x / Width * 2 - 1;
                _y = _y / Height * 2 - 1;
                cout << "Applying new coordinate to point: " << *focused_x << " -> " << _x << endl;
                *focused_x = _x;
                *focused_y = _y;
            };

            edit_click_id = Spark::loop_add(drag_update);
        }
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
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
    GLFWwindow *window = Spark::get_main_window();
    glfwGetCursorPos(window, &cx, &cy);

    cy = Height - cy;
    cx = cx / Width * 2 - 1;
    cy = cy / Height * 2 - 1;
    // найти близжайшую, находящуюся в радиусе 0.1 пикселей
    // (в системе отсчёта -1 1, -1 1),
    // вершину к курсору и запомнить треугольник, к которой
    // она принадлежит
    GLTriangle *triag = NULL;
    GLfloat *vertex_x = NULL;
    GLfloat *vertex_y = NULL;
    GLfloat min_distance = 99999;
    for (auto &t : colored_groups[active_group].triangles) {
        // без взятия корня для скорости
        GLfloat distance = pow(t.v1.x - cx, 2) + pow(t.v1.y - cy, 2);
        if (distance < min_distance && distance < 0.01) {
            min_distance = distance;
            vertex_x = &t.v1.x;
            vertex_y = &t.v1.y;
        }

        distance = pow(t.v2.x - cx, 2) + pow(t.v2.y - cy, 2);
        if (distance < min_distance && distance < 0.01) {
            min_distance = distance;
            vertex_x = &t.v2.x;
            vertex_y = &t.v2.y;
        }

        distance = pow(t.v3.x - cx, 2) + pow(t.v3.y - cy, 2);
        if (distance < min_distance && distance < 0.01) {
            min_distance = distance;
            vertex_x = &t.v3.x;
            vertex_y = &t.v3.y;
        }
    }
    cout << "Found some dot nearby" << endl;

    focused_x = vertex_x;
    focused_y = vertex_y;
}
void edit_mode_clicked (Spark::Button *btn) {
    is_edit_mode = !is_edit_mode;
    cout << "Is edit mode?: " << is_edit_mode << endl;
    if (is_edit_mode) {
        edit_loop_id = Spark::loop_add(edit_loop_func);
    } else {
        Spark::loop_remove(edit_loop_id);
        edit_loop_id = -1;
    }
}

int main(void) {
    if (!glfwInit())
        return -1;

    GLFWwindow* window = glfwCreateWindow(Width, Height, "Triangle", NULL, NULL);
    Spark::init(window);

    if (!window) {
        glfwTerminate();
        return -1;
    }
    colored_groups.push_back({});

    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 32; x++) {
            active_mask[y][x] = (y-x)%2;
        }
    }

    // UI
    glfwMakeContextCurrent(window);
    auto main_box = Spark::SidePane(Spark::START, 120, 10);
    drawing_bounds = Spark::Rect(120, 0, Width-120, Height);

    auto next_group = Spark::Button(100, 40);
    next_group.set_margin(10, 10, 10, 0);
    main_box.add_child(&next_group);

    auto rslider = Spark::Slider(100, 40);
    rslider.set_margin(10, 0, 10, 0);
    main_box.add_child(&rslider);

    auto gslider = Spark::Slider(100, 40);
    gslider.set_margin(10, 0, 10, 0);
    main_box.add_child(&gslider);

    auto bslider = Spark::Slider(100, 40);
    bslider.set_margin(10, 0, 10, 0);
    main_box.add_child(&bslider);

    auto rm_primitive = Spark::Button(100, 40);
    rm_primitive.set_margin(10, 0, 10, 0);
    main_box.add_child(&rm_primitive);

    auto rm_group = Spark::Button(100, 40);
    rm_group.set_margin(10, 0, 10, 0);
    main_box.add_child(&rm_group);

    auto edit_mode = Spark::Button(100, 40);
    edit_mode.set_margin(10, 0, 10, 0);
    main_box.add_child(&edit_mode);

    Spark::add_mouse_callback(edit_click_func);
    Spark::add_mouse_callback(Mouse);
    next_group.clicked_connect(next_group_clicked);
    rslider.clicked_connect(rslider_changed);
    gslider.clicked_connect(gslider_changed);
    bslider.clicked_connect(bslider_changed);
    rm_primitive.clicked_connect(rm_primitive_clicked);
    rm_group.clicked_connect(rm_group_clicked);
    edit_mode.clicked_connect(edit_mode_clicked);
    // UI end

    while (!glfwWindowShouldClose(window))
    {
        Spark::loop_iterate();
        display();
        main_box.render();

        glfwSwapBuffers(window);
        glfwWaitEvents();
    }

    glfwTerminate();
    return 0;
}
