#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include "SparkGUI/spark_core.hpp"
#include "SparkGUI/spark_gui.hpp"
#include <iostream>
#include <ostream>
#include <vector>

using namespace std;

GLint Width = 512, Height = 512;

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
    // for (const auto group : colored_groups) {
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
}

bool Mouse(GLFWwindow* window, int button, int action, int mods) {
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
    cout << "Red value: " << component << endl;
    colored_groups[active_group].color.r =  component;
}
void gslider_changed (Spark::Slider *slider) {
    auto component = slider->get_value();
    cout << "Green value: " << component << endl;
    colored_groups[active_group].color.g =  component;
}
void bslider_changed (Spark::Slider *slider) {
    auto component = slider->get_value();
    cout << "Blue value: " << component << endl;
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

    Spark::add_mouse_callback(Mouse);
    next_group.clicked_connect(next_group_clicked);
    rslider.clicked_connect(rslider_changed);
    gslider.clicked_connect(gslider_changed);
    bslider.clicked_connect(bslider_changed);
    rm_primitive.clicked_connect(rm_primitive_clicked);
    rm_group.clicked_connect(rm_group_clicked);
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
