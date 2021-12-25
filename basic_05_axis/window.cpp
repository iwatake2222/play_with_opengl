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
/*** Include ***/
/* for general */
#include <cstdint>
#include <cstdio>
#include <fstream> 
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

#include "window.h"
#include "transform.h"

/*** Macro ***/
/* macro function */
#define RUN_CHECK(x)                                         \
  if (!(x)) {                                                \
    fprintf(stderr, "Error at %s:%d\n", __FILE__, __LINE__); \
    exit(1);                                                 \
  }

/* Setting */
static constexpr float KEY_SPEED = 3.0f; // 3 units / second
static constexpr float MOUSE_ROT_SPEED = 0.005f;
static constexpr float MOUSE_MOV_SPEED = 0.05f;
static constexpr float MOUSE_WHEEL_SPEED = 1.0f;

/*** Global variable ***/


/*** Function ***/
void Window::CbResize(GLFWwindow* window, int32_t width, int32_t height)
{
    int32_t fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);

    Window* const instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (instance) {
        instance->m_width = width;
        instance->m_height = height;
    }
}

void Window::CbWheel(GLFWwindow* window, double x, double y)
{
    Window* const instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (instance) {
        Matrix mat_trans = Transform::Translate(0.0f, 0.0f, static_cast<float>(y) * MOUSE_WHEEL_SPEED);
        instance->m_mat_view = mat_trans * instance->m_mat_view;
    }
}

void Window::CbKeyboard(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
{
    Window* const instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (instance) {
        if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
            instance->m_is_darkmode = !instance->m_is_darkmode;
        }
    }
}

Matrix Window::GetViewProjection(float fovy, float z_near, float z_far)
{
    const float aspect = static_cast<float>(m_width) / m_height;
    const Matrix projection = Projection::Perspective(fovy, aspect, z_near, z_far);
    return projection * m_mat_view;
}

Window::Window(int32_t width, int32_t height, const char* title)
{
    /* Initialize variables */
    m_width = width;
    m_height = height;
    m_is_darkmode = true;
    m_mat_view = Transform::LookAt(0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    /* Create a window (x4 anti-aliasing, OpenGL3.3 Core Profile)*/
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    RUN_CHECK(m_window = glfwCreateWindow(width, height, title, nullptr, nullptr));
    glfwMakeContextCurrent(m_window);

    /* Initialize GLEW. This must be after initializing GLFW, creating window and setting current context window */
    glewExperimental = true;
    RUN_CHECK(glewInit() == GLEW_OK);

    /* Enable Backface Culling (Don't draw backface) */
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    /* enable Depth buffer */
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    /* Sync buffer swap timing with vsync */
    glfwSwapInterval(1);

    /* Store this pointer to window */
    glfwSetWindowUserPointer(m_window, this);

    /* Ensure not to miss input */
    glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(m_window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);

    /* Set callback */
    glfwSetWindowSizeCallback(m_window, CbResize);
    CbResize(m_window, width, height);

    glfwSetScrollCallback(m_window, CbWheel);
    glfwSetKeyCallback(m_window, CbKeyboard);

    m_last_time = glfwGetTime();
    glfwGetCursorPos(m_window, &m_last_mouse_x, &m_last_mouse_y);
}

Window::~Window()
{
    glfwDestroyWindow(m_window);
}

void Window::LookAt(const Matrix& eye, const Matrix& gaze, const Matrix& up)
{
    m_mat_view = Transform::LookAt(eye, gaze, up);
}

bool Window::FrameStart()
{
    if (!m_window) return false;
    if (glfwWindowShouldClose(m_window) == GL_TRUE) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
        return false;
    }
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwPollEvents();

    double current_time = glfwGetTime();
    float delta_time = float(current_time - m_last_time);
    m_last_time = current_time;

    double mouse_x, mouse_y;
    glfwGetCursorPos(m_window, &mouse_x, &mouse_y);
    float mouse_move_x = (float)(mouse_x - m_last_mouse_x);
    float mouse_move_y = (float)(mouse_y - m_last_mouse_y);
    m_last_mouse_x = mouse_x;
    m_last_mouse_y = mouse_y;

    Matrix mat_rot = Matrix::Identity(4);
    Matrix mat_trans = Matrix::Identity(4);
    if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_2) != GLFW_RELEASE) {
        mat_rot = Transform::RotateY(mouse_move_x * MOUSE_ROT_SPEED) * Transform::RotateX(mouse_move_y * MOUSE_ROT_SPEED);
    }
    if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_3) != GLFW_RELEASE) {
        mat_trans = Transform::Translate(mouse_move_x * MOUSE_MOV_SPEED, -mouse_move_y * MOUSE_MOV_SPEED, 0.0f) * mat_trans;
    }
    
    if (glfwGetKey(m_window, GLFW_KEY_W) != GLFW_RELEASE) {
        mat_trans = Transform::Translate(0.0f, 0.0f, delta_time * KEY_SPEED) * mat_trans;
    } else if (glfwGetKey(m_window, GLFW_KEY_S) != GLFW_RELEASE) {
        mat_trans = Transform::Translate(0.0f, 0.0f, -delta_time * KEY_SPEED) * mat_trans;
    }
    if (glfwGetKey(m_window, GLFW_KEY_A) != GLFW_RELEASE) {
        mat_trans = Transform::Translate(delta_time * KEY_SPEED, 0.0f, 0.0f) * mat_trans;
    } else if (glfwGetKey(m_window, GLFW_KEY_D) != GLFW_RELEASE) {
        mat_trans = Transform::Translate(-delta_time * KEY_SPEED, 0.0f, 0.0f) * mat_trans;
    }
    if (glfwGetKey(m_window, GLFW_KEY_Z) != GLFW_RELEASE) {
        mat_trans = Transform::Translate(0.0f, delta_time * KEY_SPEED, 0.0f) * mat_trans;
    } else if (glfwGetKey(m_window, GLFW_KEY_X) != GLFW_RELEASE) {
        mat_trans = Transform::Translate(0.0f, -delta_time * KEY_SPEED, 0.0f) * mat_trans;
    }
    
    /* view matrix = rotation * translate. So no need to move the camera to the origin */
    /* note: tx, ty, tz in view matrix are camera coordinate */
    m_mat_view = mat_trans * m_mat_view;
    //Matrix t_to_org = Transform::Translate(-m_mat_view(0, 3), -m_mat_view(1, 3), -m_mat_view(2, 3));
    //Matrix t_from_org = Transform::Translate(m_mat_view(0, 3), m_mat_view(1, 3), m_mat_view(2, 3));
    //m_mat_view = t_to_org * m_mat_view;
    m_mat_view = mat_rot * m_mat_view;
    //m_mat_view = t_from_org * m_mat_view;
    

    if (m_is_darkmode) {
        glClearColor(0.1f, 0.1f, 0.1f, 0.0f); 
    } else {
        glClearColor(0.9f, 0.9f, 0.9f, 0.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return true;
}


void Window::SwapBuffers()
{
    glfwSwapBuffers(m_window);
}