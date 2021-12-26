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
#include <array>

/* for GLFW */
#include <GL/glew.h>     /* this must be before including glfw*/
#include <GLFW/glfw3.h>

#include "matrix.h"

class Window
{
private:
    static void CbResize(GLFWwindow* window, int32_t width, int32_t height);
    static void CbWheel(GLFWwindow* window, double x, double y);
    static void CbKeyboard(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods);
public:
    Window(int32_t width = 720, int32_t height = 480, const char* title = "test");
    ~Window();
    void LookAt(const std::array<float, 3>& eye, const std::array<float, 3>& gaze, const std::array<float, 3>& up);
    bool FrameStart();
    void SwapBuffers();
    Matrix GetViewProjection(float fovy = 1.0f, float z_near = 0.1f, float z_far = 1000.0f);

private:
    void MoveCameraPosFromCameraCoordinate(float dx, float dy, float dz);


private:
    GLFWwindow* m_window;
    int32_t m_width;
    int32_t m_height;
    std::array<float, 3> m_camera_pos;  // in world coordinate
    std::array<float, 3> m_camera_angle;

    double m_last_time;
    double m_last_mouse_x;
    double m_last_mouse_y;

    bool m_is_darkmode;
};


#endif
