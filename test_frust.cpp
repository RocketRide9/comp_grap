#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cassert>
#include <chrono>
#include <cmath>
#include <concepts>
#include <cstdio>
#include <functional>
#include <iterator>
#include <type_traits>
#include "SparkGUI/spark.hpp"

double Width = 1024;
double Height = 700;

using namespace std;

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

void display() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    camera_look = camera_pos + camera_dir * 10;
    gluLookAt(camera_pos.x, camera_pos.y, camera_pos.z,
              camera_look.x, camera_look.y, camera_look.z,
              camera_up.x,  camera_up.y,  camera_up.z);


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1, 1, 1, 1);
    glColor3f(0.0, 0.0, 0.0);
    glPointSize(2);
    glLineWidth(5);

    glEnable(GL_DEPTH_TEST);
    glBegin(GL_LINES);
        glColor3f(1., 0, 0);
        glVertex3f(0, 0, 0); glVertex3f(200, 0, 0);
        glColor3f(0, 1., 0);
        glVertex3f(0, 0, 0); glVertex3f(0, 200, 0);
        glColor3f(0, 0, 1.);
        glVertex3f(0, 0, 0); glVertex3f(0, 0, 200);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.75, 0.1, 0.90);
    glVertex3f(100, 100, 100); glVertex3f(200, 100, 100); glVertex3f(200, 200, 100); glVertex3f(100, 200, 100);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.6, 0, 0.90);
    glVertex3f(200, 100, 100); glVertex3f(200, 100, 200); glVertex3f(200, 200, 200); glVertex3f(200, 200, 100);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.1, 0.9, 0.1, 1);
    glVertex3f(100, 200, 100); glVertex3f(100, 100, 100); glVertex3f(100, 100, 200); glVertex3f(100, 200, 200);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.6, 0, 0.90);
    glVertex3f(100, 200, 100);  glVertex3f(200, 200, 100); glVertex3f(200, 200, 200); glVertex3f(100, 200, 200);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.6, 0.5, 0.90);
    glVertex3f(100, 100, 100); glVertex3f(100, 100, 200); glVertex3f(200, 100, 200); glVertex3f(200, 100, 100);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.75, 0, 0.20);
    glVertex3f(100, 100, 200); glVertex3f(200, 100, 200); glVertex3f(200, 200, 200); glVertex3f(100, 200, 200);
    glEnd();
    glDisable(GL_DEPTH_TEST);
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

    GLFWwindow* window = glfwCreateWindow(Width, Height, "Triangle", NULL, NULL);
    Spark::init(window);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, keyboard_callback);

    glEnable ( GL_LIGHTING );
    glEnable ( GL_LIGHT0 );
    float pos[] = { 0, 0, 50, 1 };
    float mat[] = { 0.2, 0.2, 0.2, 1 };
    glLightModelf ( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
    glLightfv ( GL_LIGHT0, GL_POSITION, pos );
    glMaterialfv ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat );
    glEnable ( GL_COLOR_MATERIAL );

    // glRotatef(180, 0, +-1, 0);
    // glTranslatef(0, 0, -250);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // glOrtho(0.0, Width, Height, 0, 100, 200);
    glFrustum(-Width/32, Width/32, -Height/32, Height/32, 25, 1000);

    // glViewport(200, 0, Width, Height);

    // glOrtho(drawing_bounds.x1, drawing_bounds.x2, drawing_bounds.y2, drawing_bounds.y1, -200, 200);

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
