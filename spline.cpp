#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include "SparkGUI/text.hpp"

using namespace std;

GLint Width = 1000, Height = 1000;

int start_point_x = -9;
int start_point_y = -9;
int end_point_x = 9;
int end_point_y = 9;
int middle_point = 0;

int points_count = 0;

float posit_x = 0.0f;
float posit_y = 0.0f;

float scale = 1.0f;

struct Vec3f {
    float x, y, z;
};

struct GLPoint {
    GLfloat x, y;

    GLPoint operator*(float const& rhs) const {
        auto n = GLPoint(*this);

        n.x *= rhs;
        n.y *= rhs;

        return n;
    }

    GLPoint operator/(float const& rhs) const {
        auto n = GLPoint(*this);

        n.x /= rhs;
        n.y /= rhs;

        return n;
    }

    GLPoint operator+(GLPoint const& rhs) const {
        auto n = GLPoint(*this);

        n.x += rhs.x;
        n.y += rhs.y;

        return n;
    }

    GLPoint operator-(GLPoint const& rhs) const {
        auto n = GLPoint(*this);

        n.x -= rhs.x;
        n.y -= rhs.y;

        return n;
    }
};

Vec3f graph_line_color = { 0, 0, 0 };
int graph_line_size = 2;
vector <GLPoint> PixelCoord;
vector <GLPoint> PointCoord;
vector <GLPoint> SplineGraph;

// тип изменяемого объекта
enum ObjType {
    MARKS,
    GRAPH,

    OBJS_NUM
} target_obj = GRAPH;

enum ParamType {
    COLOR,
    SIZE,

    PARAMS_NUM
} target_param = SIZE;

enum ColorComponent {
    RED,
    GREEN,
    BLUE,

    COMPONENTS_NUM
} target_component = RED;

// показывать ломаную
bool show_polyline = false;
bool show_median = false;

double PartMultipLagrange(double x, int i, int j) {
    return (x - PointCoord[j].x) / (PointCoord[i].x - PointCoord[j].x);
}

double MultipLagrange(double x, int i) {
    double result = 1;
    for (int j = 0; j < PointCoord.size(); j++) {
        if (i == j) continue;
        result *= PartMultipLagrange(x, i, j);
    }
    return result;
}

double PolynomLagrange(double x) {
    double result = 0;
    for (int i = 0; i < PointCoord.size(); i++) {
        result += PointCoord[i].y * MultipLagrange(x, i);
    }
    return result;
}

string NumToString(float num) {
    string str = to_string(num);
    if (num == int(num)) {
        auto pos = str.find('.');
        while (str.length() != pos) {
            auto s = str.length();
            str.pop_back();
        }
    }
    return str;
}

int mark_type = 1;
int marks_size = 5;
Vec3f marks_color = { 1, 0, 0 };
const int marks_count = 2;
// Нарисовать отметки для массива PixelCoord
// середины отметок совпадают с координатами точек 
void draw_marks(int type, int size, Vec3f color) {

    switch (type) {
        // квадраты
    case 0:
        glPointSize(size);
        glColor3f(color.x, color.y, color.z);
        glBegin(GL_POINTS);
        for (int i = 0; i < (int)PixelCoord.size(); i++) {
            glVertex2f(PixelCoord[i].x + posit_x * scale, PixelCoord[i].y + posit_y * scale);
        }
        glEnd();
        break;
        // треугольники
    case 1: {
        glPointSize(1);
        glColor3f(color.x, color.y, color.z);
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < (int)PixelCoord.size(); i++) {
            auto coord = PixelCoord[i] + GLPoint(posit_x * scale, posit_y * scale);

            auto v1 = coord - GLPoint(size / 2. / Width * 4, size / 2. / Height * 4) * scale;
            auto v2 = v1 + GLPoint(size / (float)Width * 4., 0) * scale;
            auto v3 = coord + GLPoint(0, size / 2. / Height * 4) * scale;
            glVertex2f(v1.x, v1.y);
            glVertex2f(v2.x, v2.y);
            glVertex2f(v3.x, v3.y);
        }
        glEnd();
        break;
    }
    default:
        fprintf(stderr, "Неверный тип отметки");
        exit(1);
        break;
    }
}

void draw_status() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, Width, Height, 0, 0, 1);

    string status = "";

    switch (target_obj) {
    case MARKS:
        status += "Marks: ";
        break;
    case GRAPH:
        status += "Graph line: ";
        break;
    case OBJS_NUM:
        exit(1);
        break;
    }

    switch (target_param) {
    case COLOR:
        switch (target_component) {
        case RED:
            status += "Red component";
            break;
        case GREEN:
            status += "Green component";
            break;
        case BLUE:
            status += "Blue component";
            break;
        case COMPONENTS_NUM:
            break;
        }
        break;
    case SIZE:
        status += "size";
        break;
    case PARAMS_NUM:
        exit(1);
        break;
    }

    glScalef(2, 2, 0);
    print_string(10, 10, status.c_str(), 0.6, 0, 1);

    glPopMatrix();
}

// Функция для настройки и отображения координатной плоскости
void display() {

    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1, 1, 1, 1);
    glColor3f(0.0, 0.0, 0.0);

    //Рисуем координатные оси
    glLineWidth(3);
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 1.0);

    glVertex2f(-1.0f, posit_y);
    glVertex2f(1.0f, posit_y);

    glVertex2f(posit_x, -1.0f);
    glVertex2f(posit_x, 1.0f);

    glEnd();

    glLineWidth(graph_line_size);
    glColor3f(0, 0, 0);
    glBegin(GL_LINES);

    if (show_median) {
        GLfloat median_value;
        int middle = PointCoord.size() / 2;
        if (PointCoord.size() % 2 == 0) {
            median_value = (PointCoord[middle - 1].y + PointCoord[middle].y) / 2.;
        }
        else {
            median_value = PointCoord[middle].y;
        }
        glVertex2f(-1.0f, median_value / (10 * scale) + posit_y);
        glVertex2f(1.0f, median_value / (10 * scale) + posit_y);
    }

    glEnd();

    //Рисуем разбиения для красивой разметки
    glLineWidth(1);
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 1.0);
    float h_x = -1.0;
    float h_y = -1.0;
    while (h_x < 1.0) {
        h_x += 1.0 / 10.0;
        glVertex2f(h_x, -1.0);
        glVertex2f(h_x, 1.0);
    }
    while (h_y < 1.0) {
        h_y += 1.0 / 10.0;
        glVertex2f(-1.0, h_y);
        glVertex2f(1.0, h_y);
    }
    glEnd();

    //Рисуем числовые и координатные подписи 
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, Width, Height, 0, 0, 1);

    glScalef(2.0, 2.0, 1.0); //масштабирование для задания крупных подписей
    print_string((Width - 25.0) / 2.0, (Height / 2.0 - posit_y * Height / 2.0 + 10.0) / 2.0, "X", 0, 0, 0);
    print_string((Width / 2.0 + posit_x * Width / 2.0 - 16.0) / 2.0, 5.0, "Y", 0, 0, 0);
    string str;
    const char* result;
    h_x = -1.0;
    for (int i = start_point_x; i < middle_point; i++) {
        h_x += 1.0 / 10.0;
        float value_scaling = i * scale;
        str = NumToString(value_scaling);
        result = str.c_str();
        print_string(
            ((h_x + 1) * Width / 2.0 - 5.0 - str.length() * 6.0) / 2.0,
            (Height / 2.0 - posit_y * Height / 2.0 + 10.0) / 2.0,
            result,
            0, 0, 0
        );
    }

    h_x += 1.0 / 10.0;
    str = to_string(middle_point);
    result = str.c_str();
    print_string(
        (Width / 2.0 + posit_x * Width / 2.0 - 15.0) / 2.0,
        (Height / 2.0 - posit_y * Height / 2.0 + 10.0) / 2.0,
        result, 0, 0, 0
    );

    for (int i = middle_point + 1; i < end_point_x + 1; i++) {
        h_x += 1.0 / 10.0;
        float value_scaling = i * scale;
        str = NumToString(value_scaling);
        result = str.c_str();
        print_string(
            ((h_x + 1) * Width / 2.0 + 1 - str.length() * 6.0) / 2.0,
            (Height / 2.0 - posit_y * Height / 2.0 + 10.0) / 2.0,
            result, 0, 0, 0
        );
    }

    h_y = -1.0;
    for (int i = start_point_y; i < middle_point; i++) {
        h_y += 1.0 / 10.0;
        float value_scaling = -i * scale;
        str = NumToString(value_scaling);
        result = str.c_str();
        print_string(
            (Width / 2.0 + posit_x * Width / 2.0 - 3 - str.length() * 12.0) / 2.0,
            ((h_y + 1) * Height / 2.0 - 7.0) / 2.0,
            result,
            0, 0, 0
        );
    }
    h_y += 1.0 / 10.0;
    for (int i = middle_point + 1; i < end_point_y + 1; i++) {
        h_y += 1.0 / 10.0;
        float value_scaling = -i * scale;
        str = NumToString(value_scaling);
        result = str.c_str();
        print_string(
            (Width / 2.0 + posit_x * Width / 2.0 - 3 - str.length() * 12.0) / 2.0,
            ((h_y + 1) * Height / 2.0 - 7.0) / 2.0,
            result, 0, 0, 0
        );
    }
    glPopMatrix();

    //Масштабирование
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glScalef(1.0 / scale, 1.0 / scale, 1.0);

    //Расчет координат графика
    SplineGraph.clear();
    if (PointCoord.size() > 1) {
        for (int i = 0; i < PointCoord.size() - 1; i++) {
            double x0 = PointCoord[i].x, x1 = PointCoord[i + 1].x;
            double dh = (x1 - x0);
            int SplineSplit = dh * 100.0 / scale; // количество разбиений интервала
            dh /= SplineSplit;
            for (int j = 0; j <= SplineSplit; j++) {
                GLPoint p;
                p.x = x0 + j * dh;
                p.y = PolynomLagrange(p.x);
                SplineGraph.push_back(p);
            }
        }

        //Вывод графика на экран
        glLineWidth(graph_line_size);
        glColor3f(graph_line_color.x, graph_line_color.y, graph_line_color.z);
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < SplineGraph.size(); i++)
            glVertex2f(
                SplineGraph[i].x / 10.0 + posit_x * scale,
                SplineGraph[i].y / 10.0 + posit_y * scale
            );
        glEnd();

        // Вывод ломаной кривой
        if (show_polyline) {
            glLineWidth(graph_line_size);
            glColor3f(0, 0, 0);
            glBegin(GL_LINE_STRIP);
            for (int i = 0; i < (int)PointCoord.size(); i++) {
                glVertex2f(
                    PointCoord[i].x / 10.0 + posit_x * scale,
                    PointCoord[i].y / 10.0 + posit_y * scale
                );
            }
            glEnd();
        }
    }

    draw_marks(mark_type, marks_size, marks_color);
    glPopMatrix();

    draw_status();
}

void ConvertPixelToCoord(GLPoint p) {
    p.x *= 10.0; p.y *= 10.0;
    PointCoord.push_back(p);
    sort(
        PointCoord.begin(), PointCoord.end(), [](const GLPoint& a, const GLPoint& b)
        {return a.x < b.x; }
    );
    cout << "Point coord (x, y): (" << PointCoord[points_count - 1].x
        << "," << PointCoord[points_count - 1].y << ")\n";
}

void reshape(GLFWwindow* window, int width, int height) {
    Width = width; Height = height;
    glViewport(0, 0, Width, Height);
}

void mouse(GLFWwindow* window, int button, int action, int mode) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double _x, _y;
        glfwGetCursorPos(window, &_x, &_y);
        _y = Height - _y;
        GLPoint p;
        p.x = (_x / Width * 2 - 1 - posit_x) * scale;
        p.y = (_y / Height * 2 - 1 - posit_y) * scale;

        PixelCoord.push_back(p);
        points_count++;
        sort(
            PixelCoord.begin(), PixelCoord.end(), [](const GLPoint& a, const GLPoint& b)
            {return a.x < b.x; }
        );

        cout << "\nPixel coord (x, y): (" << p.x << "," << p.y << ")\n";
        ConvertPixelToCoord(p);
        if (points_count < 2)
            cout << "\nNot enough points to create a spline" << endl;
    }
}

void key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)action;
    (void)scancode;
    (void)mods;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key == GLFW_KEY_W && action != GLFW_RELEASE) {
        start_point_y -= 2;
        end_point_y -= 2;
        posit_y -= 0.2f;
    }
    if (key == GLFW_KEY_S && action != GLFW_RELEASE) {
        start_point_y += 2;
        end_point_y += 2;
        posit_y += 0.2f;
    }
    if (key == GLFW_KEY_A && action != GLFW_RELEASE) {
        start_point_x -= 2;
        end_point_x -= 2;
        posit_x += 0.2f;
    }
    if (key == GLFW_KEY_D && action != GLFW_RELEASE) {
        start_point_x += 2;
        end_point_x += 2;
        posit_x -= 0.2f;
    }
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        scale /= 2;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        scale *= 2;
    }

    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
        mark_type = (mark_type + 1) % marks_count;
    }

    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        target_obj = (ObjType)((target_obj + 1) % OBJS_NUM);
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        target_param = (ParamType)((target_param + 1) % PARAMS_NUM);
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        target_component = RED;
    }
    if (key == GLFW_KEY_G && action == GLFW_PRESS) {
        target_component = GREEN;
    }
    if (key == GLFW_KEY_B && action == GLFW_PRESS) {
        target_component = BLUE;
    }

    if ((key == GLFW_KEY_EQUAL || key == GLFW_KEY_MINUS) && action == GLFW_PRESS) {
        // использовать знак, чтобы не обрабатывать по-отдельности 
        // нажатие на - и = (+ без шифта)
        int sign = 1;
        if (key == GLFW_KEY_EQUAL) {
            sign = 1;
        }
        if (key == GLFW_KEY_MINUS) {
            sign = -1;
        }

        switch (target_obj) {
        case MARKS:
            switch (target_param) {
            case COLOR:
                switch (target_component) {
                case RED:
                    marks_color.x += sign * 0.1;
                    break;
                case GREEN:
                    marks_color.y += sign * 0.1;
                    break;
                case BLUE:
                    marks_color.z += sign * 0.1;
                    break;
                case COMPONENTS_NUM:
                    exit(1);
                    break;
                }
                break;
            case SIZE:
                marks_size += sign * 1;
                break;
            case PARAMS_NUM:
                exit(1);
                break;
            }
            break;
        case GRAPH:
            switch (target_param) {
            case COLOR:
                switch (target_component) {
                case RED:
                    graph_line_color.x += sign * 0.1;
                    break;
                case GREEN:
                    graph_line_color.y += sign * 0.1;
                    break;
                case BLUE:
                    graph_line_color.z += sign * 0.1;
                    break;
                case COMPONENTS_NUM:
                    break;
                }
                break;
            case SIZE:
                graph_line_size += sign * 1;
                break;
            case PARAMS_NUM:
                break;
            }
            break;
        case OBJS_NUM:
            exit(1);
            break;
        }
    }
    if (key == GLFW_KEY_COMMA && action == GLFW_PRESS) {
        show_polyline = !show_polyline;
    }
    if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        show_median = !show_median;
    }
}

int main() {
    if (!glfwInit()) {
        cout << "Error init GLFW" << endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(Width, Height, "Coordinate Plane", NULL, NULL);
    if (!window) {
        glfwTerminate();
        cout << "Error creation window" << endl;
        return -1;
    }

    glfwMakeContextCurrent(window);
    glViewport(0, 0, Width, Height);
    glfwSetFramebufferSizeCallback(window, reshape);
    glfwSetKeyCallback(window, key);
    glfwSetMouseButtonCallback(window, mouse);

    while (!glfwWindowShouldClose(window)) {
        display();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
