#ifndef WINDOW_H
#define WINDOW_H

/*** Include ***/
/* for general */
#include <cstdint>
#include <cstdio>
#include <vector>

/* for GLFW */
#include <GL/glew.h>     /* this must be before including glfw*/
#include <GLFW/glfw3.h>

#include "matrix.h"

class Window
{
private:
    static void CbResize(GLFWwindow* window, int32_t width, int32_t height);
    static void CbWheel(GLFWwindow* window, double x, double y);
public:
    Window(int32_t width = 720, int32_t height = 480, const char* title = "test");
    ~Window();
    bool FrameStart();
    void SwapBuffers();
    Matrix GetViewProjection(float fovy = 1.0f, float z_near = 1.0f, float z_far = 1000.0f);

private:
    GLFWwindow* m_window;
    int32_t m_width;
    int32_t m_height;
    Matrix m_camera_position;   // vec3
    Matrix m_camera_target;   // vec3
    Matrix m_camera_up;   // vec3
};


#endif
