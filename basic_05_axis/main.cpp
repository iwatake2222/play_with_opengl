/*** Include ***/
/* for general */
#include <cstdint>
#include <cstdio>
#include <fstream> 
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

/* for GLFW */
#include <GL/glew.h>     /* this must be before including glfw*/
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp> 

#include "matrix.h"
#include "transform.h"
#include "shader.h"

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



class Object
{
public:
    struct Vertex
    {
        GLfloat position[3];
        GLfloat color[3];
    };
public:
    Object(GLuint attr_index, GLint elem_size, GLsizei vertex_num, const Vertex* vertex, GLsizei index_num = 0, const GLuint* index = nullptr);
    virtual ~Object();
    void Bind() const;
private:
    Object(const Object& object);   // not allowed
    Object& operator=(const Object& object);    // not allowed
private:
    GLuint vao_;
    GLuint vbo_;
    GLuint ibo_;
};

Object::Object(GLuint attr_index, GLint elem_size, GLsizei vertex_num, const Vertex* vertex, GLsizei index_num, const GLuint* index)
{
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, vertex_num * sizeof(Vertex), vertex, GL_STATIC_DRAW);
    glVertexAttribPointer(attr_index, elem_size, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<Vertex*>(0)->position);
    glEnableVertexAttribArray(attr_index);
    glVertexAttribPointer(1, elem_size, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<Vertex*>(0)->color);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &ibo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_num * sizeof(GLuint), index, GL_STATIC_DRAW);
}

Object::~Object()
{
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ibo_);
}

void Object::Bind() const
{
    glBindVertexArray(vao_);
}


class Shape
{
public:
    Shape(GLuint attr_index, GLint elem_size, GLsizei vertex_num, const Object::Vertex* vertex, GLsizei index_num = 0, const GLuint* index = nullptr);
    void Draw() const;
private:
    virtual void Execute() const;

protected:
    const GLsizei vertex_num_;
private:
    std::shared_ptr<const Object> object_;
};

Shape::Shape(GLuint attr_index, GLint elem_size, GLsizei vertex_num, const Object::Vertex* vertex, GLsizei index_num, const GLuint* index)
    : object_(new Object(attr_index, elem_size, vertex_num, vertex, index_num, index)), vertex_num_(vertex_num)
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


class SolidShape : public Shape
{
public:
    SolidShape(GLuint attr_index, GLint elem_size, GLsizei vertex_num, const Object::Vertex* vertex);
private:
    virtual void Execute() const;
};

SolidShape::SolidShape(GLuint attr_index, GLint elem_size, GLsizei vertex_num, const Object::Vertex* vertex)
    : Shape(attr_index, elem_size, vertex_num, vertex)
{
    //
}
void SolidShape::Execute() const
{
    glDrawArrays(GL_TRIANGLES, 0, vertex_num_);
}



class ShapeIndex : public Shape
{
public:
    ShapeIndex(GLuint attr_index, GLint elem_size, GLsizei vertex_num, const Object::Vertex* vertex, GLsizei index_num, const GLuint* index);
private:
    virtual void Execute() const;
protected:
    const GLsizei index_num;
};

ShapeIndex::ShapeIndex(GLuint attr_index, GLint elem_size, GLsizei vertex_num, const Object::Vertex* vertex, GLsizei index_num, const GLuint* index)
    : Shape(attr_index, elem_size, vertex_num, vertex, index_num, index), index_num(index_num)
{
    //
}
void ShapeIndex::Execute() const
{
    glDrawElements(GL_LINES, index_num, GL_UNSIGNED_INT, 0);
}




class SolidShapeIndex : public ShapeIndex
{
public:
    SolidShapeIndex(GLuint attr_index, GLint elem_size, GLsizei vertex_num, const Object::Vertex* vertex, GLsizei index_num, const GLuint* index);
private:
    virtual void Execute() const;
protected:
    const GLsizei index_num;
};

SolidShapeIndex::SolidShapeIndex(GLuint attr_index, GLint elem_size, GLsizei vertex_num, const Object::Vertex* vertex, GLsizei index_num, const GLuint* index)
    : ShapeIndex(attr_index, elem_size, vertex_num, vertex, index_num, index), index_num(index_num)
{
    //
}
void SolidShapeIndex::Execute() const
{
    glDrawElements(GL_TRIANGLES, index_num, GL_UNSIGNED_INT, 0);
}



constexpr Object::Vertex rectangleVertex[] =
{
    { -0.5f, -0.5f },
    { 0.5f, -0.5f },
    { 0.5f, 0.5f },
    { -0.5f, 0.5f }
};
constexpr Object::Vertex octahedronVertex[] =
{
    { 0.0f, 1.0f, 0.0f },
    { -1.0f, 0.0f, 0.0f },
    { 0.0f, -1.0f, 0.0f },
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f },
    { 0.0f, -1.0f, 0.0f },
    { 0.0f, 0.0f, -1.0f },
    { -1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f },
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, -1.0f }
};

constexpr Object::Vertex cubeVertex[] =
{
{ -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f }, // (0)
{ -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.8f }, // (1)
{ -1.0f, 1.0f, 1.0f, 0.0f, 0.8f, 0.0f }, // (2)
{ -1.0f, 1.0f, -1.0f, 0.0f, 0.8f, 0.8f }, // (3)
{ 1.0f, 1.0f, -1.0f, 0.8f, 0.0f, 0.0f }, // (4)
{ 1.0f, -1.0f, -1.0f, 0.8f, 0.0f, 0.8f }, // (5)
{ 1.0f, -1.0f, 1.0f, 0.8f, 0.8f, 0.0f }, // (6)
{ 1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.8f } // (7)
};
constexpr GLuint wireCubeIndex[] =
{
1, 0, // (a)
2, 7, // (b)
3, 0, // (c)
4, 7, // (d)
5, 0, // (e)
6, 7, // (f)
1, 2, // (g)
2, 3, // (h)
3, 4, // (i)
4, 5, // (j)
5, 6, // (k)
6, 1 // (l)
};

//constexpr GLuint solidCubeIndex[] =
//{
//    0, 1, 2, 0, 2, 3, // 左
//    0, 3, 4, 0, 4, 5, // 裏
//    0, 5, 6, 0, 6, 1, // 下
//    7, 6, 5, 7, 5, 4, // 右
//    7, 4, 3, 7, 3, 2, // 上
//    7, 2, 1, 7, 1, 6 // 前
//};

//constexpr Object::Vertex solidCubeVertex[] =
//{
//    // 左
//    { -1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.1f },
//    { -1.0f, -1.0f, 1.0f, 0.1f, 0.8f, 0.1f },
//    { -1.0f, 1.0f, 1.0f, 0.1f, 0.8f, 0.1f },
//    { -1.0f, 1.0f, -1.0f, 0.1f, 0.8f, 0.1f },
//    // 裏
//    { 1.0f, -1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
//    { -1.0f, -1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
//    { -1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
//    { 1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
//    // 下
//    { -1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.8f },
//    { 1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.8f },
//    { 1.0f, -1.0f, 1.0f, 0.1f, 0.8f, 0.8f },
//    { -1.0f, -1.0f, 1.0f, 0.1f, 0.8f, 0.8f },
//    // 右
//    { 1.0f, -1.0f, 1.0f, 0.1f, 0.1f, 0.8f },
//    { 1.0f, -1.0f, -1.0f, 0.1f, 0.1f, 0.8f },
//    { 1.0f, 1.0f, -1.0f, 0.1f, 0.1f, 0.8f },
//    { 1.0f, 1.0f, 1.0f, 0.1f, 0.1f, 0.8f },
//    // 上
//    { -1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.1f },
//    { -1.0f, 1.0f, 1.0f, 0.8f, 0.1f, 0.1f },
//    { 1.0f, 1.0f, 1.0f, 0.8f, 0.1f, 0.1f },
//    { 1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.1f },
//    // 前
//    { -1.0f, -1.0f, 1.0f, 0.8f, 0.8f, 0.1f },
//    { 1.0f, -1.0f, 1.0f, 0.8f, 0.8f, 0.1f },
//    { 1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.1f },
//    { -1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.1f }
//};
//constexpr GLuint solidCubeIndex[] =
//{
//0, 1, 2, 0, 2, 3, // 左
//4, 5, 6, 4, 6, 7, // 裏
//8, 9, 10, 8, 10, 11, // 下
//12, 13, 14, 12, 14, 15, // 右
//16, 17, 18, 16, 18, 19, // 上
//20, 21, 22, 20, 22, 23 // 前
//};

constexpr Object::Vertex solidCubeVertex[] =
{
    // 左
    { -1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.1f },
    { -1.0f, -1.0f, 1.0f, 0.1f, 0.8f, 0.1f },
    { -1.0f, 1.0f, 1.0f, 0.1f, 0.8f, 0.1f },
    { -1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.1f },
    { -1.0f, 1.0f, 1.0f, 0.1f, 0.8f, 0.1f },
    { -1.0f, 1.0f, -1.0f, 0.1f, 0.8f, 0.1f },
    // 裏
    { 1.0f, -1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
    { -1.0f, -1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
    { -1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
    { 1.0f, -1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
    { -1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
    { 1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
    // 下
    { -1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.8f },
    { 1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.8f },
    { 1.0f, -1.0f, 1.0f, 0.1f, 0.8f, 0.8f },
    { -1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.8f },
    { 1.0f, -1.0f, 1.0f, 0.1f, 0.8f, 0.8f },
    { -1.0f, -1.0f, 1.0f, 0.1f, 0.8f, 0.8f },
    // 右
    { 1.0f, -1.0f, 1.0f, 0.1f, 0.1f, 0.8f },
    { 1.0f, -1.0f, -1.0f, 0.1f, 0.1f, 0.8f },
    { 1.0f, 1.0f, -1.0f, 0.1f, 0.1f, 0.8f },
    { 1.0f, -1.0f, 1.0f, 0.1f, 0.1f, 0.8f },
    { 1.0f, 1.0f, -1.0f, 0.1f, 0.1f, 0.8f },
    { 1.0f, 1.0f, 1.0f, 0.1f, 0.1f, 0.8f },
    // 上
    { -1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.1f },
    { -1.0f, 1.0f, 1.0f, 0.8f, 0.1f, 0.1f },
    { 1.0f, 1.0f, 1.0f, 0.8f, 0.1f, 0.1f },
    { -1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.1f },
    { 1.0f, 1.0f, 1.0f, 0.8f, 0.1f, 0.1f },
    { 1.0f, 1.0f, -1.0f, 0.8f, 0.1f, 0.1f },
    // 前
    { -1.0f, -1.0f, 1.0f, 0.8f, 0.8f, 0.1f },
    { 1.0f, -1.0f, 1.0f, 0.8f, 0.8f, 0.1f },
    { 1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.1f },
    { -1.0f, -1.0f, 1.0f, 0.8f, 0.8f, 0.1f },
    { 1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.1f },
    { -1.0f, 1.0f, 1.0f, 0.8f, 0.8f, 0.1f }
};
constexpr GLuint solidCubeIndex[] =
{
0, 1, 2, 3, 4, 5, // 左
6, 7, 8, 9, 10, 11, // 裏
12, 13, 14, 15, 16, 17, // 下
18, 19, 20, 21, 22, 23, // 右
24, 25, 26, 27, 28, 29, // 上
30, 31, 32, 33, 34, 35 // 前
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
    GLint modelviewprojection_loc_;
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
        instance->scale_ += static_cast<float>(y);
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
    RUN_CHECK(window_ = glfwCreateWindow(width, height, title, nullptr, nullptr));
    glfwMakeContextCurrent(window_);

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
        "uniform mat4 modelviewprojection;\n"
        "in vec4 position;\n"
        "in vec4 color;\n"
        "out vec4 vertex_color;\n"
        "void main()\n"
        "{\n"
        " vertex_color = color;\n"
        " gl_Position = modelviewprojection * position;\n"
        "}";
    static constexpr GLchar fsrc[] =
        "#version 150 core\n"
        "in vec4 vertex_color;\n"
        "out vec4 fragment;\n"
        "void main()\n"
        "{\n"
        " fragment = vertex_color;\n"
        "}\n";
    program_id_ = CreateShaderProgram(vsrc, fsrc);
    int32_t position_loc = glGetAttribLocation(program_id_, "position");
    int32_t color_loc = glGetAttribLocation(program_id_, "color");
    modelviewprojection_loc_ = glGetUniformLocation(program_id_, "modelviewprojection");
    
    /* Create shape */
    //int32_t vertex_num = sizeof(rectangleVertex) / sizeof(rectangleVertex[0]);
    //int32_t vertex_elem_size = sizeof(rectangleVertex[0].position) / sizeof(rectangleVertex[0].position[0]);
    //shape_ = std::make_unique<Shape>(position_loc, vertex_elem_size, vertex_num, rectangleVertex);
    //int32_t vertex_num = sizeof(octahedronVertex) / sizeof(octahedronVertex[0]);
    //int32_t vertex_elem_size = sizeof(octahedronVertex[0].position) / sizeof(octahedronVertex[0].position[0]);
    //shape_ = std::make_unique<Shape>(position_loc, vertex_elem_size, vertex_num, octahedronVertex);
    //uint32_t vertex_num = sizeof(cubeVertex) / sizeof(cubeVertex[0]);
    //uint32_t vertex_elem_size = sizeof(cubeVertex[0].position) / sizeof(cubeVertex[0].position[0]);
    //uint32_t index_num = sizeof(wireCubeIndex) / sizeof(wireCubeIndex[0]);
    //shape_ = std::make_unique<ShapeIndex>(position_loc, vertex_elem_size, vertex_num, cubeVertex, index_num, wireCubeIndex);
    //uint32_t vertex_num = sizeof(solidCubeVertex) / sizeof(solidCubeVertex[0]);
    //uint32_t vertex_elem_size = sizeof(solidCubeVertex[0].position) / sizeof(solidCubeVertex[0].position[0]);
    //uint32_t index_num = sizeof(solidCubeIndex) / sizeof(solidCubeIndex[0]);
    //shape_ = std::make_unique<SolidShapeIndex>(position_loc, vertex_elem_size, vertex_num, solidCubeVertex, index_num, solidCubeIndex);
    uint32_t vertex_num = sizeof(solidCubeVertex) / sizeof(solidCubeVertex[0]);
    uint32_t vertex_elem_size = sizeof(solidCubeVertex[0].position) / sizeof(solidCubeVertex[0].position[0]);
    shape_ = std::make_unique<SolidShape>(position_loc, vertex_elem_size, vertex_num, solidCubeVertex);
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
        location_x_ = static_cast<float>(x) * 2.0f / width_ - 1.0f;
        location_y_ = 1.0f - static_cast<float>(y) * 2.0f / height_;
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
    

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    

    glUseProgram(program_id_);

    const Matrix r = Transform::Rotate(static_cast<GLfloat>(glfwGetTime()), 0.0f, 1.0f, 0.0f);
    const Matrix translation = Transform::Translate(location_x_, location_y_, 0.0f);
    const Matrix model = translation * r;
    const Matrix view = Transform::LookAt(3.0f, 4.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    //float scale = scale_ * 2.0f;
    //const float w = width_ / scale;
    //const float h = height_ / scale;
    //const Matrix projection = Matrix::frustum(-w, w, -h, h, 1.0f, 10.0f);
    const float fovy = scale_ * 0.01f;
    const float aspect = static_cast<float>(width_) / height_;
    const Matrix projection = Projection::Perspective(fovy, aspect, 1.0f, 10.0f);
    const Matrix modelviewprojection = projection * view * model;

    glUniformMatrix4fv(modelviewprojection_loc_, 1, GL_TRUE, modelviewprojection.Data());
    shape_->Draw();

    const Matrix modelviewprojection2 = modelviewprojection * Transform::Translate(0.0f, 0.0f, 3.0f);
    glUniformMatrix4fv(modelviewprojection_loc_, 1, GL_TRUE, modelviewprojection2.Data());
    shape_->Draw();

    glfwSwapBuffers(window_);

    return true;
}


int main(int argc, char *argv[])
{
    /*** Initialize ***/
    /* Initialize OpenGL */
    RUN_CHECK(glfwInit() == GL_TRUE);
    std::atexit(glfwTerminate);
    glfwSetTime(0.0);

    Window my_window;
    
    /*** Start loop ***/
    while (1) {
        if (my_window.RunFrame() == false) break;
    }

    return 0;
}
