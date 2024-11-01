#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <cassert>
#include <chrono>
#include <cmath>
#include <concepts>
#include <cstdio>
#include <functional>
#include <iterator>
#include <type_traits>
#include <fstream>
#include <iostream>
#include "SparkGUI/spark.hpp"

double Width = 1024;
double Height = 700;

float sx = 1, sy = 1, sz = 1;
float _sx, _sy, _sz;

bool active_normal = 0; // нормаль активна(отображается)
bool active_smoothing_normal = 0; // нормаль сглаживается и отображается
bool active_frame = 0; // показ каркаса
bool active_tex = 0; // показ текстуры
bool active_ort_frus = 0; // по умолчанию включена перспективная проекция

using namespace std;

struct GLPoint {
    float y, z;
};

GLPoint vertex1, vertex2, vertex3, vertex4;

struct Vector {
    Vector(float _x, float _y, float _z) {
        x = _x;
        y = _y;
        z = _z;
    }
    Vector() {
        x = 0;
        y = 0;
        z = 0;
    }
    float x, y, z;
};

class Vec3 {
    public:
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {
        if (z > 0e-14) {
            pitch = atan(sqrt(x*x + y*y)/z);
        } else if (z < -0e-14) {
            pitch = atan(sqrt(x*x + y*y)/z) + M_PI;
        } else {
            pitch = M_PI_2;
        }

        if (x > 0e-14) {
            yaw = atan(y/x);
        } else if (x < -0e-14 and y > 0e-14) {
            yaw = atan(y/x) + M_PI;
        } else if (x < -0e-14 and y < -0e-14) {
            yaw = atan(y/x) - M_PI;
        } else {
            if (y > 0) {
                yaw = M_PI_2;
            } else {
                yaw = M_PI + M_PI_2;
            }
        }
        printf("yaw, pitch = %f, %f\n", yaw, pitch);
    }
    Vec3() {}
    float x = 0, y = 0, z = 0;
    float yaw, pitch;

    void add_pitch(float angle) {
        pitch += angle;
        if (pitch > M_PI) {
            pitch = M_PI;
        } else if (pitch < 0e+4) {
            pitch = 0e+4;
        }
        x = sin(pitch) * cos(yaw);
        y = sin(pitch) * sin(yaw);
        z = cos(pitch);
    }
    void add_yaw(float angle) {
        yaw += angle;
        x = sin(pitch) * cos(yaw);
        z = cos(pitch);
        y = sin(pitch) * sin(yaw);
    }

    // void rotate_y(float angle) {
    //     float new_x = x * cos(angle) + z * sin(angle);
    //     float new_z = -x * sin(angle) + z * cos(angle);

    //     x = new_x;
    //     z = new_z;
    // }
    // void rotate_x(float angle) {
    //     float new_y = y * cos(angle) - z * sin(angle);
    //     float new_z = y * sin(angle) + z * cos(angle);
    //     y = new_y;
    //     z = new_z;
    // }
    // void rotate_z(float angle) {
    //     float new_x = x * cos(angle) - y * sin(angle);
    //     float new_y = x * sin(angle) + y * cos(angle);
    //     x = new_x;
    //     y = new_y;
    // }

    Vec3 operator*(float const& rhs) const {
        auto n = Vec3(*this);

        n.x *= rhs;
        n.y *= rhs;
        n.z *= rhs;

        return n;
    }

    Vec3 operator/(float const& rhs) const {
        auto n = Vec3(*this);

        n.x /= rhs;
        n.y /= rhs;
        n.z /= rhs;

        return n;
    }

    Vec3 operator+(Vec3 const& rhs) const {
        auto n = Vec3(*this);

        n.x += rhs.x;
        n.y += rhs.y;
        n.z += rhs.z;

        return n;
    }

    Vec3& operator+=(Vec3 const& rhs) {
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;

        return *this;
    }

    Vec3& operator/=(float const& rhs) {
        this->x /= rhs;
        this->y /= rhs;
        this->z /= rhs;

        return *this;
    }

    Vec3& operator-=(Vec3 const& rhs) {
        this->x -= rhs.x;
        this->y -= rhs.y;
        this->z -= rhs.z;

        return *this;
    }

    float norm() {
        return std::sqrt(x*x + y*y + z*z);
    }
    private:
};

Vec3 camera_pos = {-150, 0, 50};
Vec3 camera_dir = {1, 0, 0};
Vec3 camera_up = {0, 0, 1};
Vec3 camera_look = camera_pos + camera_dir * 10;

Vec3 start_posit_v1, start_posit_v2, start_posit_v3, start_posit_v4; // начальное положение вершин сечения
Vec3 end_posit_v1, end_posit_v2, end_posit_v3, end_posit_v4; // конечное положение вершин сечения
Vec3 point1_replic, point2_replic; // первая и вторая точка тиражирования

float norma(Vector v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vector normal(Vector v1, Vector v2) {
    return { v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x };
}

void init() { // инициализация сечений
    start_posit_v1.x = point1_replic.x; start_posit_v1.y = vertex1.y; start_posit_v1.z = vertex1.z;
    start_posit_v2.x = point1_replic.x; start_posit_v2.y = vertex2.y; start_posit_v2.z = vertex2.z;
    start_posit_v3.x = point1_replic.x; start_posit_v3.y = vertex3.y; start_posit_v3.z = vertex3.z;
    start_posit_v4.x = point1_replic.x; start_posit_v4.y = vertex4.y; start_posit_v4.z = vertex4.z;

    float dx, dy, dz;
    dx = point2_replic.x - point1_replic.x;
    dy = point2_replic.y - point1_replic.y;
    dz = point2_replic.z - point1_replic.z;

    end_posit_v1.x = point1_replic.x + dx; end_posit_v1.y = vertex1.y + dy; end_posit_v1.z = vertex1.z + dz;
    end_posit_v2.x = point1_replic.x + dx; end_posit_v2.y = vertex2.y + dy; end_posit_v2.z = vertex2.z + dz;
    end_posit_v3.x = point1_replic.x + dx; end_posit_v3.y = vertex3.y + dy; end_posit_v3.z = vertex3.z + dz;
    end_posit_v4.x = point1_replic.x + dx; end_posit_v4.y = vertex4.y + dy; end_posit_v4.z = vertex4.z + dz;
}

void create_light() { // задание света
    // задаем точечное затухающее освещение
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    float pos[] = { 50.0 , 50.0, 50.0, 1 };

    // для серебра
    float material_ambient[] = { 0.19, 0.19, 0.19, 1.0 }; // фоновая составляющая материала
    float material_diffuse[] = { 0.5, 0.5, 0.5, 1.0 }; // диффузная составляющая материала
    float material_specular[] = { 0.5, 0.5, 0.5, 1.0 }; // зеркальная составляющая материала
    float material_shininess = 51.0; // коэффициент блеска материала

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0); // константный коеф затухания
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.004); // линейный коеф затухания

    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, material_shininess);
    glEnable(GL_COLOR_MATERIAL);
}

void init_tex() { // инициализация текстуры
    /* Чтение текстуры из файла */
    unsigned char data54[54];
    FILE* file = fopen("tex.bmp", "rb");
    fread(data54, 54, 1, file);
    int width = *(data54 + 18);
    int height = *(data54 + 22);
    GLubyte* pixels = new GLubyte[width * height * 3];
    fread(pixels, width * height * 3, 1, file);
    fclose(file);

    /* Установка параметров текстуры */
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height,
        GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
}

void create_proection() { // задание проекции
    if (!active_ort_frus) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustum(-Width / 32, Width / 32, -Height / 32, Height / 32, 25, 1000);
    }
    if (active_ort_frus) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-Width / 4, Width / 4, -Height / 4, Height / 4, 0, 1000);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display() {
    create_proection();

    camera_look = camera_pos + camera_dir * 10;
    gluLookAt(camera_pos.x, camera_pos.y, camera_pos.z,
        camera_look.x, camera_look.y, camera_look.z,
        camera_up.x, camera_up.y, camera_up.z);


    create_light();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1, 1, 1, 1);
    glPointSize(2);
    glLineWidth(5);

    glEnable(GL_DEPTH_TEST);
    glBegin(GL_LINES);
    glColor3f(1., 0, 0);
    glVertex3f(0, 0, 0); glVertex3f(400, 0, 0);
    glColor3f(0, 1., 0);
    glVertex3f(0, 0, 0); glVertex3f(0, 400, 0);
    glColor3f(0, 0, 1.);
    glVertex3f(0, 0, 0); glVertex3f(0, 0, 400);
    glEnd();


    Vector v1, v2, normal_1, normal_2, normal_3, normal_4;
    Vector sum_normal[8];
    float norm;

    glScalef(sx, sy, sz); // преобразование масштабирования

    if (active_tex) {
        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0, 1.0, 1.0); // для правильного цвета текстуры
    }
    else {
        glDisable(GL_TEXTURE_2D);
        glColor3ub(192, 192, 192); // задаем серебрянный материал
    }

    if (!active_frame) glBegin(GL_QUADS); // начальное положение сечения
    else glBegin(GL_LINE_LOOP);


    v1 = { start_posit_v4.x - start_posit_v1.x, start_posit_v4.y - start_posit_v1.y, start_posit_v4.z - start_posit_v1.z },
        v2 = { start_posit_v2.x - start_posit_v1.x, start_posit_v2.y - start_posit_v1.y, start_posit_v2.z - start_posit_v1.z };
    normal_1 = normal(v1, v2);
    sum_normal[0].x += normal_1.x; sum_normal[0].y += normal_1.y; sum_normal[0].z += normal_1.z;
    glNormal3f(normal_1.x, normal_1.y, normal_1.z);
    if (active_tex) glTexCoord2f(0, 0);
    glVertex3f(start_posit_v1.x, start_posit_v1.y, start_posit_v1.z);

    v1 = { start_posit_v1.x - start_posit_v2.x, start_posit_v1.y - start_posit_v2.y, start_posit_v1.z - start_posit_v2.z },
        v2 = { start_posit_v3.x - start_posit_v2.x, start_posit_v3.y - start_posit_v2.y, start_posit_v3.z - start_posit_v2.z };
    normal_2 = normal(v1, v2);
    sum_normal[1].x += normal_2.x; sum_normal[1].y += normal_2.y; sum_normal[1].z += normal_2.z;
    glNormal3f(normal_2.x, normal_2.y, normal_2.z);
    if (active_tex) glTexCoord2f(1, 0);
    glVertex3f(start_posit_v2.x, start_posit_v2.y, start_posit_v2.z);

    v1 = { start_posit_v2.x - start_posit_v3.x, start_posit_v2.y - start_posit_v3.y, start_posit_v2.z - start_posit_v3.z },
        v2 = { start_posit_v4.x - start_posit_v3.x, start_posit_v4.y - start_posit_v3.y, start_posit_v4.z - start_posit_v3.z };
    normal_3 = normal(v1, v2);
    sum_normal[2].x += normal_3.x; sum_normal[2].y += normal_3.y; sum_normal[2].z += normal_3.z;
    glNormal3f(normal_3.x, normal_3.y, normal_3.z);
    if (active_tex) glTexCoord2f(1, 1);
    glVertex3f(start_posit_v3.x, start_posit_v3.y, start_posit_v3.z);

    v1 = { start_posit_v3.x - start_posit_v4.x, start_posit_v3.y - start_posit_v4.y, start_posit_v3.z - start_posit_v4.z },
        v2 = { start_posit_v1.x - start_posit_v4.x, start_posit_v1.y - start_posit_v4.y, start_posit_v1.z - start_posit_v4.z };
    normal_4 = normal(v1, v2);
    sum_normal[3].x += normal_4.x; sum_normal[3].y += normal_4.y; sum_normal[3].z += normal_4.z;
    glNormal3f(normal_4.x, normal_4.y, normal_4.z);
    if (active_tex) glTexCoord2f(0, 1);
    glVertex3f(start_posit_v4.x, start_posit_v4.y, start_posit_v4.z);
    glEnd();


    // деление на масштабы для одинаковой длинны нормали, которую мы отображаем   
    if (active_normal && !active_smoothing_normal) {
        glLineWidth(2);
        glBegin(GL_LINES);
        glColor3ub(63, 63, 63); // значения, обратные цвету примитива (для лучшей видимости)


        glVertex3f(start_posit_v1.x, start_posit_v1.y, start_posit_v1.z);
        norm = norma(normal_1);
        glVertex3f(start_posit_v1.x + (30.0 * normal_1.x) / (norm * sx),
            start_posit_v1.y + (30.0 * normal_1.y) / (norm * sy),
            start_posit_v1.z + (30.0 * normal_1.z) / (norm * sz));


        glVertex3f(start_posit_v2.x, start_posit_v2.y, start_posit_v2.z);
        norm = norma(normal_2);
        glVertex3f(start_posit_v2.x + (30.0 * normal_2.x) / (norm * sx),
            start_posit_v2.y + (30.0 * normal_2.y) / (norm * sy),
            start_posit_v2.z + (30.0 * normal_2.z) / (norm * sz));


        glVertex3f(start_posit_v3.x, start_posit_v3.y, start_posit_v3.z);
        norm = norma(normal_3);
        glVertex3f(start_posit_v3.x + (30.0 * normal_3.x) / (norm * sx),
            start_posit_v3.y + (30.0 * normal_3.y) / (norm * sy),
            start_posit_v3.z + (30.0 * normal_3.z) / (norm * sz));

        glVertex3f(start_posit_v4.x, start_posit_v4.y, start_posit_v4.z);
        norm = norma(normal_4);
        glVertex3f(start_posit_v4.x + (30.0 * normal_4.x) / (norm * sx),
            start_posit_v4.y + (30.0 * normal_4.y) / (norm * sy),
            start_posit_v4.z + (30.0 * normal_4.z) / (norm * sz));
        glEnd();

    }

    glLineWidth(5);
    if (active_tex) glColor3f(1.0, 1.0, 1.0);
    else glColor3ub(192, 192, 192); // задаем серебрянный материал
    if (!active_frame) glBegin(GL_QUADS); // конечное положение сечения
    else glBegin(GL_LINE_LOOP);

    v1 = { end_posit_v4.x - end_posit_v1.x, end_posit_v4.y - end_posit_v1.y, end_posit_v4.z - end_posit_v1.z },
        v2 = { end_posit_v2.x - end_posit_v1.x, end_posit_v2.y - end_posit_v1.y, end_posit_v2.z - end_posit_v1.z };
    normal_1 = normal(v2, v1);
    sum_normal[4].x += normal_1.x; sum_normal[4].y += normal_1.y; sum_normal[4].z += normal_1.z;
    glNormal3f(normal_1.x, normal_1.y, normal_1.z);
    if (active_tex) glTexCoord2f(0, 0);
    glVertex3f(end_posit_v1.x, end_posit_v1.y, end_posit_v1.z);

    v1 = { end_posit_v1.x - end_posit_v2.x, end_posit_v1.y - end_posit_v2.y, end_posit_v1.z - end_posit_v2.z },
        v2 = { end_posit_v3.x - end_posit_v2.x, end_posit_v3.y - end_posit_v2.y, end_posit_v3.z - end_posit_v2.z };
    normal_2 = normal(v2, v1);
    sum_normal[5].x += normal_2.x; sum_normal[5].y += normal_2.y; sum_normal[5].z += normal_2.z;
    glNormal3f(normal_2.x, normal_2.y, normal_2.z);
    if (active_tex) glTexCoord2f(1, 0);
    glVertex3f(end_posit_v2.x, end_posit_v2.y, end_posit_v2.z);

    v1 = { end_posit_v2.x - end_posit_v3.x, end_posit_v2.y - end_posit_v3.y, end_posit_v2.z - end_posit_v3.z },
        v2 = { end_posit_v4.x - end_posit_v3.x, end_posit_v4.y - end_posit_v3.y, end_posit_v4.z - end_posit_v3.z };
    normal_3 = normal(v2, v1);
    sum_normal[6].x += normal_3.x; sum_normal[6].y += normal_3.y; sum_normal[6].z += normal_3.z;
    glNormal3f(normal_3.x, normal_3.y, normal_3.z);
    if (active_tex) glTexCoord2f(1, 1);
    glVertex3f(end_posit_v3.x, end_posit_v3.y, end_posit_v3.z);

    v1 = { end_posit_v3.x - end_posit_v4.x, end_posit_v3.y - end_posit_v4.y, end_posit_v3.z - end_posit_v4.z },
        v2 = { end_posit_v1.x - end_posit_v4.x, end_posit_v1.y - end_posit_v4.y, end_posit_v1.z - end_posit_v4.z };
    normal_4 = normal(v2, v1);
    sum_normal[7].x += normal_4.x; sum_normal[7].y += normal_4.y; sum_normal[7].z += normal_4.z;
    glNormal3f(normal_4.x, normal_4.y, normal_4.z);
    if (active_tex) glTexCoord2f(0, 1);
    glVertex3f(end_posit_v4.x, end_posit_v4.y, end_posit_v4.z);
    glEnd();

    if (active_normal && !active_smoothing_normal) {
        glLineWidth(2);
        glBegin(GL_LINES);
        glColor3ub(63, 63, 63); // значения, обратные цвету примитива (для лучшей видимости)

        glVertex3f(end_posit_v1.x, end_posit_v1.y, end_posit_v1.z);
        norm = norma(normal_1);
        glVertex3f(end_posit_v1.x + (30.0 * normal_1.x) / (norm * sx),
            end_posit_v1.y + (30.0 * normal_1.y) / (norm * sy),
            end_posit_v1.z + (30.0 * normal_1.z) / (norm * sz));

        glVertex3f(end_posit_v2.x, end_posit_v2.y, end_posit_v2.z);
        norm = norma(normal_2);
        glVertex3f(end_posit_v2.x + (30.0 * normal_2.x) / (norm * sx),
            end_posit_v2.y + (30.0 * normal_2.y) / (norm * sy),
            end_posit_v2.z + (30.0 * normal_2.z) / (norm * sz));

        glVertex3f(end_posit_v3.x, end_posit_v3.y, end_posit_v3.z);
        norm = norma(normal_3);
        glVertex3f(end_posit_v3.x + (30.0 * normal_3.x) / (norm * sx),
            end_posit_v3.y + (30.0 * normal_3.y) / (norm * sy),
            end_posit_v3.z + (30.0 * normal_3.z) / (norm * sz));

        glVertex3f(end_posit_v4.x, end_posit_v4.y, end_posit_v4.z);
        norm = norma(normal_4);
        glVertex3f(end_posit_v4.x + (30.0 * normal_4.x) / (norm * sx),
            end_posit_v4.y + (30.0 * normal_4.y) / (norm * sy),
            end_posit_v4.z + (30.0 * normal_4.z) / (norm * sz));
        glEnd();
    }

    glLineWidth(5);
    if (active_tex) glColor3f(1.0, 1.0, 1.0);
    else glColor3ub(192, 192, 192); // задаем серебрянный материал
    if (!active_frame) glBegin(GL_QUADS); // нижняя плоскость
    else glBegin(GL_LINE_LOOP);

    v1 = { end_posit_v1.x - start_posit_v1.x, end_posit_v1.y - start_posit_v1.y, end_posit_v1.z - start_posit_v1.z },
        v2 = { start_posit_v2.x - start_posit_v1.x, start_posit_v2.y - start_posit_v1.y, start_posit_v2.z - start_posit_v1.z };
    normal_1 = normal(v2, v1);
    sum_normal[0].x += normal_1.x; sum_normal[0].y += normal_1.y; sum_normal[0].z += normal_1.z;
    glNormal3f(normal_1.x, normal_1.y, normal_1.z);
    if (active_tex) glTexCoord2f(0, 0);
    glVertex3f(start_posit_v1.x, start_posit_v1.y, start_posit_v1.z);

    v1 = { start_posit_v1.x - start_posit_v2.x, start_posit_v1.y - start_posit_v2.y, start_posit_v1.z - start_posit_v2.z },
        v2 = { end_posit_v2.x - start_posit_v2.x, end_posit_v2.y - start_posit_v2.y, end_posit_v2.z - start_posit_v2.z };
    normal_2 = normal(v2, v1);
    sum_normal[1].x += normal_2.x; sum_normal[1].y += normal_2.y; sum_normal[1].z += normal_2.z;
    glNormal3f(normal_2.x, normal_2.y, normal_2.z);
    if (active_tex) glTexCoord2f(1, 0);
    glVertex3f(start_posit_v2.x, start_posit_v2.y, start_posit_v2.z);

    v1 = { start_posit_v2.x - end_posit_v2.x, start_posit_v2.y - end_posit_v2.y, start_posit_v2.z - end_posit_v2.z },
        v2 = { end_posit_v1.x - end_posit_v2.x, end_posit_v1.y - end_posit_v2.y, end_posit_v1.z - end_posit_v2.z };
    normal_3 = normal(v2, v1);
    sum_normal[5].x += normal_3.x; sum_normal[5].y += normal_3.y; sum_normal[5].z += normal_3.z;
    glNormal3f(normal_3.x, normal_3.y, normal_3.z);
    if (active_tex) glTexCoord2f(1, 1);
    glVertex3f(end_posit_v2.x, end_posit_v2.y, end_posit_v2.z);

    v1 = { end_posit_v2.x - end_posit_v1.x, end_posit_v2.y - end_posit_v1.y, end_posit_v2.z - end_posit_v1.z },
        v2 = { start_posit_v1.x - end_posit_v1.x, start_posit_v1.y - end_posit_v1.y, start_posit_v1.z - end_posit_v1.z };
    normal_4 = normal(v2, v1);
    sum_normal[4].x += normal_4.x; sum_normal[4].y += normal_4.y; sum_normal[4].z += normal_4.z;
    glNormal3f(normal_4.x, normal_4.y, normal_4.z);
    if (active_tex) glTexCoord2f(0, 1);
    glVertex3f(end_posit_v1.x, end_posit_v1.y, end_posit_v1.z);
    glEnd();

    if (active_normal && !active_smoothing_normal) {
        glLineWidth(2);
        glBegin(GL_LINES);
        glColor3ub(63, 63, 63); // значения, обратные цвету примитива (для лучшей видимости)

        glVertex3f(start_posit_v1.x, start_posit_v1.y, start_posit_v1.z);
        norm = norma(normal_1);
        glVertex3f(start_posit_v1.x + (30.0 * normal_1.x) / (norm * sx),
            start_posit_v1.y + (30.0 * normal_1.y) / (norm * sy),
            start_posit_v1.z + (30.0 * normal_1.z) / (norm * sz));

        glVertex3f(start_posit_v2.x, start_posit_v2.y, start_posit_v2.z);
        norm = norma(normal_2);
        glVertex3f(start_posit_v2.x + (30.0 * normal_2.x) / (norm * sx),
            start_posit_v2.y + (30.0 * normal_2.y) / (norm * sy),
            start_posit_v2.z + (30.0 * normal_2.z) / (norm * sz));

        glVertex3f(end_posit_v2.x, end_posit_v2.y, end_posit_v2.z);
        norm = norma(normal_3);
        glVertex3f(end_posit_v2.x + (30.0 * normal_3.x) / (norm * sx),
            end_posit_v2.y + (30.0 * normal_3.y) / (norm * sy),
            end_posit_v2.z + (30.0 * normal_3.z) / (norm * sz));

        glVertex3f(end_posit_v1.x, end_posit_v1.y, end_posit_v1.z);
        norm = norma(normal_4);
        glVertex3f(end_posit_v1.x + (30.0 * normal_4.x) / (norm * sx),
            end_posit_v1.y + (30.0 * normal_4.y) / (norm * sy),
            end_posit_v1.z + (30.0 * normal_4.z) / (norm * sz));
        glEnd();
    }

    glLineWidth(5);
    if (active_tex) glColor3f(1.0, 1.0, 1.0);
    else glColor3ub(192, 192, 192); // задаем серебрянный материал
    if (!active_frame) glBegin(GL_QUADS); // верхняя плоскость
    else glBegin(GL_LINE_LOOP);

    v1 = { end_posit_v4.x - start_posit_v4.x, end_posit_v4.y - start_posit_v4.y, end_posit_v4.z - start_posit_v4.z },
        v2 = { start_posit_v3.x - start_posit_v4.x, start_posit_v3.y - start_posit_v4.y, start_posit_v3.z - start_posit_v4.z };
    normal_1 = normal(v1, v2);
    sum_normal[3].x += normal_1.x; sum_normal[3].y += normal_1.y; sum_normal[3].z += normal_1.z;
    glNormal3f(normal_1.x, normal_1.y, normal_1.z);
    if (active_tex) glTexCoord2f(0, 0);
    glVertex3f(start_posit_v4.x, start_posit_v4.y, start_posit_v4.z);

    v1 = { start_posit_v4.x - start_posit_v3.x, start_posit_v4.y - start_posit_v3.y, start_posit_v4.z - start_posit_v3.z },
        v2 = { end_posit_v3.x - start_posit_v3.x, end_posit_v3.y - start_posit_v3.y, end_posit_v3.z - start_posit_v3.z };
    normal_2 = normal(v1, v2);
    sum_normal[2].x += normal_2.x; sum_normal[2].y += normal_2.y; sum_normal[2].z += normal_2.z;
    glNormal3f(normal_2.x, normal_2.y, normal_2.z);
    if (active_tex) glTexCoord2f(1, 0);
    glVertex3f(start_posit_v3.x, start_posit_v3.y, start_posit_v3.z);

    v1 = { start_posit_v3.x - end_posit_v3.x, start_posit_v3.y - end_posit_v3.y, start_posit_v3.z - end_posit_v3.z },
        v2 = { end_posit_v4.x - end_posit_v3.x, end_posit_v4.y - end_posit_v3.y, end_posit_v4.z - end_posit_v3.z };
    normal_3 = normal(v1, v2);
    sum_normal[6].x += normal_3.x; sum_normal[6].y += normal_3.y; sum_normal[6].z += normal_3.z;
    glNormal3f(normal_3.x, normal_3.y, normal_3.z);
    if (active_tex) glTexCoord2f(1, 1);
    glVertex3f(end_posit_v3.x, end_posit_v3.y, end_posit_v3.z);

    v1 = { end_posit_v3.x - end_posit_v4.x, end_posit_v3.y - end_posit_v4.y, end_posit_v3.z - end_posit_v4.z },
        v2 = { start_posit_v4.x - end_posit_v4.x, start_posit_v4.y - end_posit_v4.y, start_posit_v4.z - end_posit_v4.z };
    normal_4 = normal(v1, v2);
    sum_normal[7].x += normal_4.x; sum_normal[7].y += normal_4.y; sum_normal[7].z += normal_4.z;
    glNormal3f(normal_4.x, normal_4.y, normal_4.z);
    if (active_tex) glTexCoord2f(0, 1);
    glVertex3f(end_posit_v4.x, end_posit_v4.y, end_posit_v4.z);
    glEnd();

    if (active_normal && !active_smoothing_normal) {
        glLineWidth(2);
        glBegin(GL_LINES);
        glColor3ub(63, 63, 63); // значения, обратные цвету примитива (для лучшей видимости)

        glVertex3f(start_posit_v4.x, start_posit_v4.y, start_posit_v4.z);
        norm = norma(normal_1);
        glVertex3f(start_posit_v4.x + (30.0 * normal_1.x) / (norm * sx),
            start_posit_v4.y + (30.0 * normal_1.y) / (norm * sy),
            start_posit_v4.z + (30.0 * normal_1.z) / (norm * sz));

        glVertex3f(start_posit_v3.x, start_posit_v3.y, start_posit_v3.z);
        norm = norma(normal_2);
        glVertex3f(start_posit_v3.x + (30.0 * normal_2.x) / (norm * sx),
            start_posit_v3.y + (30.0 * normal_2.y) / (norm * sy),
            start_posit_v3.z + (30.0 * normal_2.z) / (norm * sz));

        glVertex3f(end_posit_v3.x, end_posit_v3.y, end_posit_v3.z);
        norm = norma(normal_3);
        glVertex3f(end_posit_v3.x + (30.0 * normal_3.x) / (norm * sx),
            end_posit_v3.y + (30.0 * normal_3.y) / (norm * sy),
            end_posit_v3.z + (30.0 * normal_3.z) / (norm * sz));

        glVertex3f(end_posit_v4.x, end_posit_v4.y, end_posit_v4.z);
        norm = norma(normal_4);
        glVertex3f(end_posit_v4.x + (30.0 * normal_4.x) / (norm * sx),
            end_posit_v4.y + (30.0 * normal_4.y) / (norm * sy),
            end_posit_v4.z + (30.0 * normal_4.z) / (norm * sz));
        glEnd();


    }

    glLineWidth(5);
    if (active_tex) glColor3f(1.0, 1.0, 1.0);
    else glColor3ub(192, 192, 192); // задаем серебрянный материал
    if (!active_frame) glBegin(GL_QUADS); // правая плоскость
    else glBegin(GL_LINE_LOOP);

    v1 = { end_posit_v1.x - start_posit_v1.x, end_posit_v1.y - start_posit_v1.y, end_posit_v1.z - start_posit_v1.z },
        v2 = { start_posit_v4.x - start_posit_v1.x, start_posit_v4.y - start_posit_v1.y, start_posit_v4.z - start_posit_v1.z };
    normal_1 = normal(v1, v2);
    sum_normal[0].x += normal_1.x; sum_normal[0].y += normal_1.y; sum_normal[0].z += normal_1.z;
    glNormal3f(normal_1.x, normal_1.y, normal_1.z);
    if (active_tex) glTexCoord2f(0, 0);
    glVertex3f(start_posit_v1.x, start_posit_v1.y, start_posit_v1.z);

    v1 = { start_posit_v1.x - start_posit_v4.x, start_posit_v1.y - start_posit_v4.y, start_posit_v1.z - start_posit_v4.z },
        v2 = { end_posit_v4.x - start_posit_v4.x, end_posit_v4.y - start_posit_v4.y, end_posit_v4.z - start_posit_v4.z };
    normal_2 = normal(v1, v2);
    sum_normal[3].x += normal_2.x; sum_normal[3].y += normal_2.y; sum_normal[3].z += normal_2.z;
    glNormal3f(normal_2.x, normal_2.y, normal_2.z);
    if (active_tex) glTexCoord2f(1, 0);
    glVertex3f(start_posit_v4.x, start_posit_v4.y, start_posit_v4.z);

    v1 = { start_posit_v4.x - end_posit_v4.x, start_posit_v4.y - end_posit_v4.y, start_posit_v4.z - end_posit_v4.z },
        v2 = { end_posit_v1.x - end_posit_v4.x, end_posit_v1.y - end_posit_v4.y, end_posit_v1.z - end_posit_v4.z };
    normal_3 = normal(v1, v2);
    sum_normal[7].x += normal_3.x; sum_normal[7].y += normal_3.y; sum_normal[7].z += normal_3.z;
    glNormal3f(normal_3.x, normal_3.y, normal_3.z);
    if (active_tex) glTexCoord2f(1, 1);
    glVertex3f(end_posit_v4.x, end_posit_v4.y, end_posit_v4.z);

    v1 = { end_posit_v4.x - end_posit_v1.x, end_posit_v4.y - end_posit_v1.y, end_posit_v4.z - end_posit_v1.z },
        v2 = { start_posit_v1.x - end_posit_v1.x, start_posit_v1.y - end_posit_v1.y, start_posit_v1.z - end_posit_v1.z };
    normal_4 = normal(v1, v2);
    sum_normal[4].x += normal_4.x; sum_normal[4].y += normal_4.y; sum_normal[4].z += normal_4.z;
    glNormal3f(normal_4.x, normal_4.y, normal_4.z);
    if (active_tex) glTexCoord2f(0, 1);
    glVertex3f(end_posit_v1.x, end_posit_v1.y, end_posit_v1.z);
    glEnd();
    if (active_normal && !active_smoothing_normal) {
        glLineWidth(2);
        glBegin(GL_LINES);
        glColor3ub(63, 63, 63); // значения, обратные цвету примитива (для лучшей видимости)

        glVertex3f(start_posit_v1.x, start_posit_v1.y, start_posit_v1.z);
        norm = norma(normal_1);
        glVertex3f(start_posit_v1.x + (30.0 * normal_1.x) / (norm * sx),
            start_posit_v1.y + (30.0 * normal_1.y) / (norm * sy),
            start_posit_v1.z + (30.0 * normal_1.z) / (norm * sz));

        glVertex3f(start_posit_v4.x, start_posit_v4.y, start_posit_v4.z);
        norm = norma(normal_2);
        glVertex3f(start_posit_v4.x + (30.0 * normal_2.x) / (norm * sx),
            start_posit_v4.y + (30.0 * normal_2.y) / (norm * sy),
            start_posit_v4.z + (30.0 * normal_2.z) / (norm * sz));

        glVertex3f(end_posit_v4.x, end_posit_v4.y, end_posit_v4.z);
        norm = norma(normal_3);
        glVertex3f(end_posit_v4.x + (30.0 * normal_3.x) / (norm * sx),
            end_posit_v4.y + (30.0 * normal_3.y) / (norm * sy),
            end_posit_v4.z + (30.0 * normal_3.z) / (norm * sz));

        glVertex3f(end_posit_v1.x, end_posit_v1.y, end_posit_v1.z);
        norm = norma(normal_4);
        glVertex3f(end_posit_v1.x + (30.0 * normal_4.x) / (norm * sx),
            end_posit_v1.y + (30.0 * normal_4.y) / (norm * sy),
            end_posit_v1.z + (30.0 * normal_4.z) / (norm * sz));
        glEnd();

    }

    glLineWidth(5);
    if (active_tex) glColor3f(1.0, 1.0, 1.0);
    else glColor3ub(192, 192, 192); // задаем серебрянный материал
    if (!active_frame) glBegin(GL_QUADS); // левая плоскость
    else glBegin(GL_LINE_LOOP);

    v1 = { end_posit_v2.x - start_posit_v2.x, end_posit_v2.y - start_posit_v2.y, end_posit_v2.z - start_posit_v2.z },
        v2 = { start_posit_v3.x - start_posit_v2.x, start_posit_v3.y - start_posit_v2.y, start_posit_v3.z - start_posit_v2.z };
    normal_1 = normal(v2, v1);
    sum_normal[1].x += normal_1.x; sum_normal[1].y += normal_1.y; sum_normal[1].z += normal_1.z;
    glNormal3f(normal_1.x, normal_1.y, normal_1.z);
    if (active_tex) glTexCoord2f(0, 0);
    glVertex3f(start_posit_v2.x, start_posit_v2.y, start_posit_v2.z);

    v1 = { start_posit_v2.x - start_posit_v3.x, start_posit_v2.y - start_posit_v3.y, start_posit_v2.z - start_posit_v3.z },
        v2 = { end_posit_v3.x - start_posit_v3.x, end_posit_v3.y - start_posit_v3.y, end_posit_v3.z - start_posit_v3.z };
    normal_2 = normal(v2, v1);
    sum_normal[2].x += normal_2.x; sum_normal[2].y += normal_2.y; sum_normal[2].z += normal_2.z;
    glNormal3f(normal_2.x, normal_2.y, normal_2.z);
    if (active_tex) glTexCoord2f(1, 0);
    glVertex3f(start_posit_v3.x, start_posit_v3.y, start_posit_v3.z);

    v1 = { start_posit_v3.x - end_posit_v3.x, start_posit_v3.y - end_posit_v3.y, start_posit_v3.z - end_posit_v3.z },
        v2 = { end_posit_v2.x - end_posit_v3.x, end_posit_v2.y - end_posit_v3.y, end_posit_v2.z - end_posit_v3.z };
    normal_3 = normal(v2, v1);
    sum_normal[6].x += normal_3.x; sum_normal[6].y += normal_3.y; sum_normal[6].z += normal_3.z;
    glNormal3f(normal_3.x, normal_3.y, normal_3.z);
    if (active_tex) glTexCoord2f(1, 1);
    glVertex3f(end_posit_v3.x, end_posit_v3.y, end_posit_v3.z);

    v1 = { end_posit_v3.x - end_posit_v2.x, end_posit_v3.y - end_posit_v2.y, end_posit_v3.z - end_posit_v2.z },
        v2 = { start_posit_v2.x - end_posit_v2.x, start_posit_v2.y - end_posit_v2.y, start_posit_v2.z - end_posit_v2.z };
    normal_4 = normal(v2, v1);
    sum_normal[5].x += normal_4.x; sum_normal[5].y += normal_4.y; sum_normal[5].z += normal_4.z;
    glNormal3f(normal_4.x, normal_4.y, normal_4.z);
    if (active_tex) glTexCoord2f(0, 1);
    glVertex3f(end_posit_v2.x, end_posit_v2.y, end_posit_v2.z);
    glEnd();

    if (active_normal && !active_smoothing_normal) {
        glLineWidth(2);
        glBegin(GL_LINES);
        glColor3ub(63, 63, 63); // значения, обратные цвету примитива (для лучшей видимости)

        glVertex3f(start_posit_v2.x, start_posit_v2.y, start_posit_v2.z);
        norm = norma(normal_1);
        glVertex3f(start_posit_v2.x + (30.0 * normal_1.x) / (norm * sx),
            start_posit_v2.y + (30.0 * normal_1.y) / (norm * sy),
            start_posit_v2.z + (30.0 * normal_1.z) / (norm * sz));

        glVertex3f(start_posit_v3.x, start_posit_v3.y, start_posit_v3.z);
        norm = norma(normal_2);
        glVertex3f(start_posit_v3.x + (30.0 * normal_2.x) / (norm * sx),
            start_posit_v3.y + (30.0 * normal_2.y) / (norm * sy),
            start_posit_v3.z + (30.0 * normal_2.z) / (norm * sz));

        glVertex3f(end_posit_v3.x, end_posit_v3.y, end_posit_v3.z);
        norm = norma(normal_3);
        glVertex3f(end_posit_v3.x + (30.0 * normal_3.x) / (norm * sx),
            end_posit_v3.y + (30.0 * normal_3.y) / (norm * sy),
            end_posit_v3.z + (30.0 * normal_3.z) / (norm * sz));

        glVertex3f(end_posit_v2.x, end_posit_v2.y, end_posit_v2.z);
        norm = norma(normal_4);
        glVertex3f(end_posit_v2.x + (30.0 * normal_4.x) / (norm * sx),
            end_posit_v2.y + (30.0 * normal_4.y) / (norm * sy),
            end_posit_v2.z + (30.0 * normal_4.z) / (norm * sz));
        glEnd();

    }
    if (active_normal && active_smoothing_normal) { // если сглаживаем нормали
        glLineWidth(2);
        glBegin(GL_LINES);

        glVertex3f(start_posit_v1.x, start_posit_v1.y, start_posit_v1.z);
        norm = norma(sum_normal[0]);
        glVertex3f(start_posit_v1.x + (30.0 * sum_normal[0].x) / (norm * sx),
            start_posit_v1.y + (30.0 * sum_normal[0].y) / (norm * sy),
            start_posit_v1.z + (30.0 * sum_normal[0].z) / (norm * sz));

        glVertex3f(start_posit_v2.x, start_posit_v2.y, start_posit_v2.z);
        norm = norma(sum_normal[1]);
        glVertex3f(start_posit_v2.x + (30.0 * sum_normal[1].x) / (norm * sx),
            start_posit_v2.y + (30.0 * sum_normal[1].y) / (norm * sy),
            start_posit_v2.z + (30.0 * sum_normal[1].z) / (norm * sz));

        glVertex3f(start_posit_v3.x, start_posit_v3.y, start_posit_v3.z);
        norm = norma(sum_normal[2]);
        glVertex3f(start_posit_v3.x + (30.0 * sum_normal[2].x) / (norm * sx),
            start_posit_v3.y + (30.0 * sum_normal[2].y) / (norm * sy),
            start_posit_v3.z + (30.0 * sum_normal[2].z) / (norm * sz));

        glVertex3f(start_posit_v4.x, start_posit_v4.y, start_posit_v4.z);
        norm = norma(sum_normal[3]);
        glVertex3f(start_posit_v4.x + (30.0 * sum_normal[3].x) / (norm * sx),
            start_posit_v4.y + (30.0 * sum_normal[3].y) / (norm * sy),
            start_posit_v4.z + (30.0 * sum_normal[3].z) / (norm * sz));

        glVertex3f(end_posit_v1.x, end_posit_v1.y, end_posit_v1.z);
        norm = norma(sum_normal[4]);
        glVertex3f(end_posit_v1.x + (30.0 * sum_normal[4].x) / (norm * sx),
            end_posit_v1.y + (30.0 * sum_normal[4].y) / (norm * sy),
            end_posit_v1.z + (30.0 * sum_normal[4].z) / (norm * sz));

        glVertex3f(end_posit_v2.x, end_posit_v2.y, end_posit_v2.z);
        norm = norma(sum_normal[5]);
        glVertex3f(end_posit_v2.x + (30.0 * sum_normal[5].x) / (norm * sx),
            end_posit_v2.y + (30.0 * sum_normal[5].y) / (norm * sy),
            end_posit_v2.z + (30.0 * sum_normal[5].z) / (norm * sz));

        glVertex3f(end_posit_v3.x, end_posit_v3.y, end_posit_v3.z);
        norm = norma(sum_normal[6]);
        glVertex3f(end_posit_v3.x + (30.0 * sum_normal[6].x) / (norm * sx),
            end_posit_v3.y + (30.0 * sum_normal[6].y) / (norm * sy),
            end_posit_v3.z + (30.0 * sum_normal[6].z) / (norm * sz));

        glVertex3f(end_posit_v4.x, end_posit_v4.y, end_posit_v4.z);
        norm = norma(sum_normal[7]);
        glVertex3f(end_posit_v4.x + (30.0 * sum_normal[7].x) / (norm * sx),
            end_posit_v4.y + (30.0 * sum_normal[7].y) / (norm * sy),
            end_posit_v4.z + (30.0 * sum_normal[7].z) / (norm * sz));

        glEnd();
    }
}

typedef function<float(float x)> InterpolatorFunc;

float linear(float x) {
    return x;
}
float quadratic(float x) {
    return x*x;
}

// Плохое название
// По сути структура для описания изменения какой-то величины
// с определённой скоростью и ускорением
template<typename T>
class PlayerSpeed {
    public:
    T start;
    T end;
    T current; // start + interpolator(progress) * (end-start)

    T v; // скорость - приращение значения за секунду
    T a; // цскорение - приращение скорости за сек

    bool arrived = true;

    void tick(float secs) {
        if (arrived) {
            return;
        }
        T smth = end - start;
        if ((smth > 0 && current > end) || (smth < 0 && current < end)) {
            arrived = true;
            printf("tightening: start: %f, current: %f, end: %f\n", start, current, end);
            current = end;
        } else {
            // printf("%f += %f\n", current, v*secs);
            current += v*secs + a*secs*secs / 2;
        }
    }

    void move(T end, T speed, T acceleration) {
        this->end += end;
        this->v = speed;
        this->a = acceleration;

        arrived = false;
        start = current;
    }

    PlayerSpeed<T> (T start)
        : start(start)
    {
        current = start;
    }

    private:
    PlayerSpeed<T>();
};

Vec3 step_direction = Vec3(0, 0, 0);

const float step_target_speed = 100;
const float step_acceleration = 450;
const float step_deacceleration = 250;

const float yaw_target_speed = M_PI/3;
const float yaw_acceleration = M_PI;
const float yaw_deacceleration = M_PI;

const float pitch_target_speed = M_PI/4;
const float pitch_acceleration = M_PI;
const float pitch_deacceleration = M_PI;

PlayerSpeed<float> step_model(0);
PlayerSpeed<float> sidestep_model(0);
PlayerSpeed<float> yaw_model(0);
PlayerSpeed<float> pitch_model(0);


bool movement_sentry (chrono::time_point<chrono::steady_clock> last_update) {
    auto dt = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - last_update).count();
    float d_secs = dt / 1000.;
    step_model.tick(d_secs);
    sidestep_model.tick(d_secs);
    pitch_model.tick(d_secs);
    yaw_model.tick(d_secs);

    auto temp = camera_dir;
    temp.z = 0;
    temp.pitch = M_PI_2;
    temp /= temp.norm();
    step_direction = temp;

    auto sidestep = step_direction;
    sidestep.add_yaw(M_PI_2);

    camera_pos += step_direction * step_model.current * d_secs;
    camera_pos += sidestep * sidestep_model.current * d_secs;
    camera_dir.add_pitch(pitch_model.current * d_secs);
    camera_dir.add_yaw(yaw_model.current * d_secs);

    return true;
}

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_W) {
        if (action == GLFW_PRESS) {
            step_model.move(step_target_speed, step_acceleration, 0);
        } else if (action == GLFW_RELEASE) {
            step_model.move(-step_target_speed, -step_deacceleration, 0);
        }
    }
    if (key == GLFW_KEY_S) {
        if (action == GLFW_PRESS) {
            step_model.move(-step_target_speed, -step_acceleration, 0);
        } else if (action == GLFW_RELEASE) {
            step_model.move(step_target_speed, step_deacceleration, 0);
        }
    }
    if (key == GLFW_KEY_UP) {
        // camera_dir.add_pitch(-M_PI_4/10);
        if (action == GLFW_PRESS) {
            pitch_model.move(-pitch_target_speed, -pitch_acceleration, 0);
        } else if (action == GLFW_RELEASE) {
            pitch_model.move(pitch_target_speed, pitch_deacceleration, 0);
        }
    }
    if (key == GLFW_KEY_DOWN) {
        if (action == GLFW_PRESS) {
            pitch_model.move(pitch_target_speed, pitch_acceleration, 0);
        } else if (action == GLFW_RELEASE) {
            pitch_model.move(-pitch_target_speed, -pitch_deacceleration, 0);
        }
    }
    if (key == GLFW_KEY_LEFT /*&& action == GLFW_PRESS*/) {
        if (action == GLFW_PRESS) {
            yaw_model.move(yaw_target_speed, yaw_acceleration, 0);
        } else if (action == GLFW_RELEASE) {
            yaw_model.move(-yaw_target_speed, -yaw_deacceleration, 0);
        }
    }
    if (key == GLFW_KEY_RIGHT /*&& action == GLFW_PRESS*/) {
        if (action == GLFW_PRESS) {
            yaw_model.move(-yaw_target_speed, -yaw_acceleration, 0);
        } else if (action == GLFW_RELEASE) {
            yaw_model.move(yaw_target_speed, yaw_deacceleration, 0);
        }
    }
    if (key == GLFW_KEY_A /*&& action == GLFW_PRESS*/) {
        if (action == GLFW_PRESS) {
            sidestep_model.move(step_target_speed, step_acceleration, 0);
        } else if (action == GLFW_RELEASE) {
            sidestep_model.move(-step_target_speed, -step_deacceleration, 0);
        }
    }
    if (key == GLFW_KEY_D /*&& action == GLFW_PRESS*/) {
        if (action == GLFW_PRESS) {
            sidestep_model.move(-step_target_speed, -step_acceleration, 0);
        } else if (action == GLFW_RELEASE) {
            sidestep_model.move(step_target_speed, step_deacceleration, 0);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        sx *= _sx; // увеличение параметра масштаба по x в считываемое из файлов кол-во раз  
    }
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        sx /= _sx; // уменьшение параметра масштаба по x в считываемое из файлов кол-во раз
    }
    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        sy *= _sy; // увеличение параметра масштаба по y в считываемое из файлов кол-во раз  
    }
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        sy /= _sy; // уменьшение параметра масштаба по y в считываемое из файлов кол-во раз
    }
    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        sz *= _sz; // увеличение параметра масштаба по z в считываемое из файлов кол-во раз
    }
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        sz /= _sz; // уменьшение параметра масштаба по z в считываемое из файлов кол-во раз
    }
    if (key == GLFW_KEY_N) {
        if (action == GLFW_PRESS) {
            active_normal = !active_normal;
        }
    }
    if (key == GLFW_KEY_M) {
        if (action == GLFW_PRESS) {
            active_smoothing_normal = !active_smoothing_normal;
        }
    }
    if (key == GLFW_KEY_F) {
        if (action == GLFW_PRESS) {
            active_frame = !active_frame;
        }
    }
    if (key == GLFW_KEY_T) {
        if (action == GLFW_PRESS) {
            active_tex = !active_tex;
        }
    }
    if (key == GLFW_KEY_O) {
        if (action == GLFW_PRESS) {
            active_ort_frus = !active_ort_frus;
        }
    }
    // if (key == GLFW_KEY_SPACE) {
    //
    //     glRotatef(15, 0, 1, 0);
    // }
    // if (key == GLFW_KEY_E) {
    //     glMatrixMode(GL_MODELVIEW);
    //     glRotatef(-15, 0, 1, 0);
    // }
}

int main() {
    if (!glfwInit())
        return -1;

    ifstream koord_2D;
    koord_2D.open("2D_section_koord.txt");
    if (koord_2D.is_open() == 0) {
        cout << "File 2D_section_koord.txt is not found" << endl;
        return 1;
    }
    koord_2D >> vertex1.y >> vertex1.z; //
    koord_2D >> vertex2.y >> vertex2.z; // 2д координаты сечения в плоскости x
    koord_2D >> vertex3.y >> vertex3.z; //
    koord_2D >> vertex4.y >> vertex4.z; //
    koord_2D.close();

    ifstream koord_replic;
    koord_replic.open("3D_trajectory_replication.txt");
    if (koord_replic.is_open() == 0) {
        cout << "File 3D_trajectory_replication.txt is not found" << endl;
        return 1;
    }
    koord_replic >> point1_replic.x >> point1_replic.y >> point1_replic.z; // 3д координаты траектории (заданной отрезком) тиражирования
    koord_replic >> point2_replic.x >> point2_replic.y >> point2_replic.z; //
    koord_replic.close();

    ifstream scale_param;
    scale_param.open("scale_param.txt");
    if (scale_param.is_open() == 0) {
        cout << "File scale_param.txt is not found" << endl;
        return 1;
    }
    scale_param >> _sx >> _sy >> _sz; // параметры масштаба
    scale_param.close();

    glfwWindowHint(GLFW_DEPTH_BITS, 24); // запрашиваем 24-битный буфер глубины

    GLFWwindow* window = glfwCreateWindow(Width, Height, "3D", NULL, NULL);
    Spark::init(window);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, keyboard_callback);

    // glRotatef(180, 0, +-1, 0);
    // glTranslatef(0, 0, -250);

    init_tex();

    // glViewport(200, 0, Width, Height);

    // glOrtho(drawing_bounds.x1, drawing_bounds.x2, drawing_bounds.y2, drawing_bounds.y1, -200, 200);

    init();
    Spark::loop_add(movement_sentry);
    while (!glfwWindowShouldClose(window))
    {
        Spark::loop_iterate();

        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
        // glfwWaitEvents();
    }

    glfwTerminate();
    return 0;
}
