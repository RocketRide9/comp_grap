﻿#include <iostream>
#include <string>
#include <vector>
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

struct GLPoint {
    GLfloat x, y;
};

vector <GLPoint> PixelCoord;
vector <GLPoint> PointCoord;
vector <GLPoint> SplineGraph;
 
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

    //Рисуем разбиения для красивой разметки
    glLineWidth(1);
    glBegin(GL_LINES);
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

    glScalef(2.0, 2.0, 1.0);
    print_string(
        (Width - 25.0) / 2.0,
        (Height / 2.0 - posit_y * Height / 2.0 + 10.0) / 2.0,
        "X",
        0, 0, 0
    );
    print_string(
        (Width / 2.0 + posit_x * Width / 2.0 - 16.0) / 2.0,
        5.0,
        "Y",
        0, 0, 0
    );
    string str;
    const char* result;
    h_x = -1.0;
    for (int i = start_point_x; i < middle_point; i++) {
        h_x += 1.0 / 10.0;
        str = to_string(i);
        result = str.c_str();
        if (str.length() == 2) {
            print_string(
                ((h_x + 1) * Width / 2.0 - 17.0) / 2.0,
                (Height / 2.0 - posit_y * Height / 2.0 + 10.0) / 2.0,
                result,
                0, 0, 0
            );
        } else {
            print_string(
                ((h_x + 1) * Width / 2.0 - 23.0) / 2.0,
                (Height / 2.0 - posit_y * Height / 2.0 + 10.0) / 2.0,
                result,
                0, 0, 0
            );
        }
    }

    h_x += 1.0 / 10.0;
    str = to_string(middle_point);
    result = str.c_str();
    print_string(
        (Width / 2.0 + posit_x * Width / 2.0 - 15.0) / 2.0,
        (Height / 2.0 - posit_y * Height / 2.0 + 10.0) / 2.0,
        result,
        0, 0, 0
    );

    for (int i = middle_point + 1; i < end_point_x + 1; i++) {
        h_x += 1.0 / 10.0;
        str = to_string(i);
        result = str.c_str();
        if (str.length() == 1) {
            print_string(
                ((h_x + 1) * Width / 2.0 - 5.0) / 2.0,
                (Height / 2.0 - posit_y * Height / 2.0 + 10.0) / 2.0,
                result, 0, 0, 0
            );
        } else {
            print_string(
                ((h_x + 1) * Width / 2.0 - 11.0) / 2.0,
                (Height / 2.0 - posit_y * Height / 2.0 + 10.0) / 2.0,
                result,
                0, 0, 0
            );
        }
    }

    h_y = -1.0;
    for (int i = start_point_y; i < middle_point; i++) {
        h_y += 1.0 / 10.0;
        str = to_string(-i);
        result = str.c_str();
        if(str.length() == 1) {
            print_string(
                (Width / 2.0 + posit_x * Width / 2.0 - 15.0) / 2.0,
                ((h_y + 1) * Height / 2.0 - 7.0) / 2.0,
                result,
                0, 0, 0
            );
        } else {
            print_string(
                (Width / 2.0 + posit_x * Width / 2.0 - 27.0) / 2.0,
                ((h_y + 1) * Height / 2.0 - 7.0) / 2.0,
                result,
                0, 0, 0
            );
        } 
    }
    h_y += 1.0 / 10.0;
    for (int i = middle_point + 1; i < end_point_y + 1; i++) {
        h_y += 1.0 / 10.0;
        str = to_string(-i);
        result = str.c_str();
        if (str.length() == 2) {
            print_string(
                (Width / 2.0 + posit_x * Width / 2.0 - 27.0) / 2.0,
                ((h_y + 1) * Height / 2.0 - 7.0) / 2.0,
                result,
                0, 0, 0
            );
        }
        else {
            print_string(
                (Width / 2.0 + posit_x * Width / 2.0 - 39.0) / 2.0,
                ((h_y + 1) * Height / 2.0 - 7.0) / 2.0,
                result,
                0, 0, 0
            );
        }
    }
    glPopMatrix();

    //Отрисовка точек
    glPointSize(5);
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_POINTS);
    for (int i = 0; i < PixelCoord.size(); i++) {
        glVertex2f(PixelCoord[i].x + posit_x, PixelCoord[i].y + posit_y);  
    }
    glEnd();

    //Расчет координат графика
    SplineGraph.clear();
    if (PointCoord.size() > 1) {
        int SplineSplit = 1000; // количество разбиений интервала
        for (int i = 0; i < PointCoord.size() - 1; i++) {
            double x0 = PointCoord[i].x, x1 = PointCoord[i + 1].x;
            double dh = (x1 - x0) / SplineSplit;
            for (int j = 0; j <= SplineSplit; j++) {
                GLPoint p;
                p.x = x0 + j * dh;
                p.y = PolynomLagrange(p.x);
                //p.y = Height - p.y;
                SplineGraph.push_back(p);
            }
        }

        //Вывод графика на экран
        glLineWidth(2);
        glColor3f(0.0, 0.0, 0.0);
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < SplineGraph.size(); i++) {
            glVertex2f(
                SplineGraph[i].x / 10.0 + posit_x,
                SplineGraph[i].y / 10.0 + posit_y
            );
        }
        glEnd();
    }

}

void ConvertPointToCoord(GLPoint p) {
    p.x *= 10.0; p.y *= 10.0;
    PointCoord.push_back(p);
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
        p.x = _x / Width * 2 - 1 - posit_x;
        p.y = _y / Height * 2 - 1 - posit_y;

        PixelCoord.push_back(p);
        points_count++;
        cout << "\nPixel coord (x, y): (" << p.x << "," << p.y << ")\n";
        ConvertPointToCoord(p);
    }
}

void key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        start_point_y -= 2;
        end_point_y -= 2;
        posit_y -= 0.2f;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        start_point_y += 2;
        end_point_y += 2;
        posit_y += 0.2f;
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        start_point_x -= 2;        
        end_point_x -= 2;
        posit_x += 0.2f;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        start_point_x += 2;
        end_point_x += 2;
        posit_x -= 0.2f;
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
