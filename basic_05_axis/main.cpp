/*** Include ***/
/* for general */
#include <cstdint>
#include <cstdio>
#include <fstream> 
#include <vector>
#include <string>
#include <memory>

/* for GLFW */
#include <GL/glew.h>     /* this must be before including glfw*/
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp> 

#include "shader.h"
#include "camera_control.h"
#include "data.h"

/*** Macro ***/
/* macro functions */
#define RUN_CHECK(x)                                         \
  if (!(x)) {                                                \
    fprintf(stderr, "Error at %s:%d\n", __FILE__, __LINE__); \
    exit(1);                                                 \
  }

/* Settings */

/*** Global variables ***/


/*** Function ***/
class Object
{
public:
    struct Vertex
    {
        GLfloat position[2];
    };
public:
    Object(GLuint attr_index, GLint elem_size, GLsizei vertex_num, const Vertex* vertex);
    virtual ~Object();
    void Bind() const;
private:
    Object(const Object& object);   // not allowed
    Object& operator=(const Object& object);    // not allowed
private:
    GLuint vao_;
    GLuint vbo_;
};

Object::Object(GLuint attr_index, GLint elem_size, GLsizei vertex_num, const Vertex* vertex)
{
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertex_num * sizeof(Vertex), vertex, GL_STATIC_DRAW);
    glVertexAttribPointer(attr_index, elem_size, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(attr_index);
}

Object::~Object()
{
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
}

void Object::Bind() const
{
    glBindVertexArray(vao_);
}


class Shape
{
public:
    Shape(GLuint attr_index, GLint elem_size, GLsizei vertex_num, const Object::Vertex* vertex);
    void Draw() const;
private:
    virtual void Execute() const;

protected:
    
    const GLsizei vertex_num_;
private:
    std::shared_ptr<const Object> object_;
};

Shape::Shape(GLuint attr_index, GLint elem_size, GLsizei vertex_num, const Object::Vertex* vertex)
    : object_(new Object(attr_index, elem_size, vertex_num, vertex)), vertex_num_(vertex_num)
{
    //
}

void Shape::Draw() const
{
    object_->Bind();
    Execute();
}

void Shape::Execute() const
{
    glDrawArrays(GL_LINE_LOOP, 0, vertex_num_);
}


constexpr Object::Vertex rectangleVertex[] =
{
    { -0.5f, -0.5f },
    { 0.5f, -0.5f },
    { 0.5f, 0.5f },
    { -0.5f, 0.5f }
};

class Window
{
private:
    static void CbResize(GLFWwindow* const window, int32_t width, int32_t height);
    static void CbWheel(GLFWwindow* window, double x, double y);
public:
    Window(int32_t width = 720, int32_t height = 480, const char* title = "test");
    ~Window();
    bool RunFrame();
    float GetAspect() const { return static_cast<float>(width_) / height_; }
private:
    GLFWwindow* window_;
    float scale_;
    int32_t width_;
    int32_t height_;
    GLuint program_id_;
    GLint size_loc_;
    GLint scale_loc_;
    GLint location_loc_;
    std::unique_ptr<Shape> shape_;
    float location_x_;
    float location_y_;
    
public:
};

void Window::CbResize(GLFWwindow* const window, int32_t width, int32_t height)
{
    int32_t fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);


    Window* const instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (instance) {
        instance->width_ = width;
        instance->height_ = height;
    }
}

void Window::CbWheel(GLFWwindow* window, double x, double y)
{
    Window* const instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (instance) {
        instance->scale_ += y;
    }
}


Window::Window(int32_t width, int32_t height, const char* title)
{
    scale_ = 100.0f;
    location_x_ = location_y_ = 0.0f;

    /* Create a window (x4 anti-aliasing, OpenGL3.3 Core Profile)*/
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    RUN_CHECK(window_ = glfwCreateWindow(width, height, title, NULL, NULL));
    glfwMakeContextCurrent(window_);

    /* Initialize GLEW. This must be after initializing GLFW, creating window and setting current context window */
    glewExperimental = true;
    RUN_CHECK(glewInit() == GLEW_OK);

    /* Sync buffer swap timing with vsync */
    glfwSwapInterval(1);

    /* Store this pointer to window */
    glfwSetWindowUserPointer(window_, this);

    /* Ensure not to miss input */
    glfwSetInputMode(window_, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window_, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);

    /* Set callback */
    glfwSetWindowSizeCallback(window_, CbResize);
    CbResize(window_, width, height);

    glfwSetScrollCallback(window_, CbWheel);

    /* Load Shader Program */
    static const GLchar vsrc[] =
        "#version 150 core\n"
        "uniform vec2 size;\n"
        "uniform float scale;\n"
        "uniform vec2 location;\n"
        "in vec4 position;\n"
        "void main()\n"
        "{\n"
        " gl_Position = vec4(2.0 * scale / size, 1.0, 1.0) * position + vec4(location, 0.0, 0.0);\n"
        "}";
    static constexpr GLchar fsrc[] =
        "#version 150 core\n"
        "out vec4 fragment;\n"
        "void main()\n"
        "{\n"
        " fragment = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";
    program_id_ = CreateShaderProgram(vsrc, fsrc);
    int32_t position_loc = glGetAttribLocation(program_id_, "position");
    size_loc_ = glGetUniformLocation(program_id_, "size");
    scale_loc_ = glGetUniformLocation(program_id_, "scale");
    location_loc_ = glGetUniformLocation(program_id_, "location");
    
    /* Create shape */
    shape_ = std::make_unique<Shape>(position_loc, 2, 4, rectangleVertex);
}

Window::~Window()
{
    glfwDestroyWindow(window_);
}


bool Window::RunFrame()
{
    if (!window_) return false;
    if (glfwWindowShouldClose(window_) == GL_TRUE) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
        return false;
    }
    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        return false;
    }
    glfwMakeContextCurrent(window_);

    glfwPollEvents();
    if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_1) != GLFW_RELEASE) {
        double x, y;
        glfwGetCursorPos(window_, &x, &y);
        location_x_ = x * 2.0f / width_ - 1.0f;
        location_y_ = 1.0f - y * 2.0f / height_;
        //printf("%f %f\n", location_x_, location_y_);
    }
    if (glfwGetKey(window_, GLFW_KEY_LEFT) != GLFW_RELEASE) {
        location_x_ -= 2.0f / width_;
    } else if (glfwGetKey(window_, GLFW_KEY_RIGHT) != GLFW_RELEASE) {
        location_x_ += 2.0f / width_;
    }
    if (glfwGetKey(window_, GLFW_KEY_DOWN) != GLFW_RELEASE) {
        location_y_ -= 2.0f / height_;
    } else if (glfwGetKey(window_, GLFW_KEY_UP) != GLFW_RELEASE) {
        location_y_ += 2.0f / height_;
    }
    

    glClearColor(0.5f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    

    glUseProgram(program_id_);
    GLfloat size[2] = { width_, height_ };
    GLfloat location[2] = { location_x_, location_y_ };
    glUniform2fv(size_loc_, 1, size);
    glUniform1f(scale_loc_, scale_);
    glUniform2fv(location_loc_, 1, location);
    shape_->Draw();


    glfwSwapBuffers(window_);

    return true;
}

int main(int argc, char *argv[])
{
    /*** Initialize ***/
    /* Initialize GLFW */
    RUN_CHECK(glfwInit() == GL_TRUE);
    std::atexit(glfwTerminate);

    Window my_window;

    /*** Start loop ***/
    while (1) {
        if (my_window.RunFrame() == false) break;
    }

    return 0;
    

    /* Create a window (x4 anti-aliasing, OpenGL3.3 Core Profile)*/
    GLFWwindow* window;
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    RUN_CHECK(window = glfwCreateWindow(720, 480, "main", NULL, NULL));
    glfwMakeContextCurrent(window);
    
    /* Initialize GLEW */
    glewExperimental = true;
    RUN_CHECK(glewInit() == GLEW_OK);

    /* Ensure not to miss input */
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);

    /* Dark blue background */
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    
    /* Enable depth test */
    glEnable(GL_DEPTH_TEST);
    /* Accept fragment if it closer to the camera than the former one */
    glDepthFunc(GL_LESS);
    /* Cull triangles which normal is not towards the camera */
    glEnable(GL_CULL_FACE);

    /* Load shader */
    GLuint programId = LoadShaderProgram("resource/SimpleVertexShader.vertexshader", "resource/SimpleFragmentShader.fragmentshader");
    GLuint matrixId = glGetUniformLocation(programId, "MVP");

    /* Create Vertex Array Object */
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    /* Create Vertex Buffer Object and copy data */
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    GLuint colorBuffer;
    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

    /* Initialize camera matrix controls (Initial position : on +Z, toward -Z) */
    CameraControl_initialize(window, glm::vec3(0, 0, 5), 3.14f, 0.0f);

    /*** Start loop ***/
    while(1) {
        my_window.RunFrame();

        glfwMakeContextCurrent(window);
        /* Clear the screen */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(programId);
        /* Camera matrix */
        CameraControl_update(window);
        glm::mat4 Projection = CameraControl_getProjectionMatrix();
        glm::mat4 View = CameraControl_getViewMatrix();

        /* Model matrix */
        glm::mat4 Model = glm::mat4(1.0f);
        static float rotX = 0.0f;
        glm::mat4 myRotationAxis = glm::rotate(rotX++ / (2 * 3.14f), glm::vec3(1, 0, 0));
        Model = myRotationAxis * Model;

        /* Calculate ModelViewProjection matrix and send it to shader */
        glm::mat4 MVP = Projection * View * Model;
        glUniformMatrix4fv(matrixId, 1, GL_FALSE, &MVP[0][0]);

        /* Set attribute buffer */
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        /* Draw the triangle */
        glDrawArrays(GL_TRIANGLES, 0, sizeof(g_vertex_buffer_data) / sizeof(float) / 3);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glUseProgram(0);

        /* Swap buffers */
        glfwSwapBuffers(window);
        glfwPollEvents();

        /* Check if the ESC key was pressed or the window was closed */
        if (glfwWindowShouldClose(window) != 0 || glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            break;
        }
    }

    /*** Finalize ***/
    /* Cleanup VBO and shader */
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &colorBuffer);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(programId);

    /* Close OpenGL window and terminate GLFW */
    glfwTerminate();

    return 0;
}
