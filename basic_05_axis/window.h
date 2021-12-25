/* Copyright 2021 iwatake2222

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
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
    void LookAt(const Matrix& eye, const Matrix& gaze, const Matrix& up);
    bool FrameStart();
    void SwapBuffers();
    Matrix GetViewProjection(float fovy = 1.0f, float z_near = 1.0f, float z_far = 1000.0f);

private:
    GLFWwindow* m_window;
    int32_t m_width;
    int32_t m_height;
    Matrix m_mat_view;       // 4 x 4

    double m_last_time;
    double m_last_mouse_x;
    double m_last_mouse_y;
};


#endif
