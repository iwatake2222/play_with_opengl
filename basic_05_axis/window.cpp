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

/*** Global variable ***/


/*** Function ***/
void Window::CbResize(GLFWwindow* const window, int32_t width, int32_t height)
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
        instance->m_camera_position[2] += static_cast<float>(y);
    }
}

Matrix Window::GetViewProjection(float fovy, float z_near, float z_far)
{
    const Matrix view = Transform::LookAt(m_camera_position, m_camera_target, m_camera_up);
    const float aspect = static_cast<float>(m_width) / m_height;
    const Matrix projection = Projection::Perspective(fovy, aspect, z_near, z_far);
    return projection * view;
}

Window::Window(int32_t width, int32_t height, const char* title)
{
    /* Initialize variables */
    m_width = width;
    m_height = height;
    m_camera_position = Matrix(3, 1, { 3.0f, 4.0f, 5.0f });
    m_camera_target = Matrix(3, 1, { 0.0f, 0.0f, 0.0f });
    m_camera_up = Matrix(3, 1, { 0.0f, 1.0f, 0.0f });

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
}

Window::~Window()
{
    glfwDestroyWindow(m_window);
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
    if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_1) != GLFW_RELEASE) {
        double x, y;
        glfwGetCursorPos(m_window, &x, &y);
        m_camera_position[0] = static_cast<float>(x) * 2.0f / m_width - 1.0f;
        m_camera_position[1] = 1.0f - static_cast<float>(y) * 2.0f / m_height;
        //printf("%f %f\n", m_mouse_x, m_mouse_y);
    }
    if (glfwGetKey(m_window, GLFW_KEY_LEFT) != GLFW_RELEASE) {
        m_camera_position[0] -= 2.0f / m_width;
    } else if (glfwGetKey(m_window, GLFW_KEY_RIGHT) != GLFW_RELEASE) {
        m_camera_position[0] += 2.0f / m_width;
    }
    if (glfwGetKey(m_window, GLFW_KEY_DOWN) != GLFW_RELEASE) {
        m_camera_position[1] -= 2.0f / m_height;
    } else if (glfwGetKey(m_window, GLFW_KEY_UP) != GLFW_RELEASE) {
        m_camera_position[2] += 2.0f / m_height;
    }

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return true;
}


void Window::SwapBuffers()
{
    glfwSwapBuffers(m_window);
}