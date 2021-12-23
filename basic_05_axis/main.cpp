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
class MyWorld
{
private:
public:
    MyWorld();
    ~MyWorld();
    bool RunFrame();
private:
    GLFWwindow* window_;
    GLuint program_id_;
public:
};



MyWorld::MyWorld()
{
    /* Initialize GLFW */
    RUN_CHECK(glfwInit() == GL_TRUE);
    std::atexit(glfwTerminate);

    /* Create a window (x4 anti-aliasing, OpenGL3.3 Core Profile)*/
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    RUN_CHECK(window_ = glfwCreateWindow(720, 480, "MyWorld", NULL, NULL));
    glfwMakeContextCurrent(window_);
    
    /* Initialize GLEW. This must be after initializing GLFW, creating window and setting current context window */
    glewExperimental = true;
    RUN_CHECK(glewInit() == GLEW_OK);

    /* swap buffer at vsync */
    glfwSwapInterval(1);


    static const GLchar vsrc[] =
        "#version 330 core\n"
        "in vec4 position;\n"
        "void main()\n"
        "{\n"
        " gl_Position = position;\n"
        "}";
    static constexpr GLchar fsrc[] =
        "#version 330 core\n"
        "out vec4 fragment;\n"
        "void main()\n"
        "{\n"
        " fragment = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";
    program_id_ = CreateShaderProgram(vsrc, fsrc);
    glBindAttribLocation(program_id_, 0, "position");
    glBindFragDataLocation(program_id_, 0, "fragment");
}

MyWorld::~MyWorld()
{
    
}




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

bool MyWorld::RunFrame()
{
    if (!window_) return false;
    if (glfwWindowShouldClose(window_) == GL_TRUE) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
        return false;
    }
    glfwMakeContextCurrent(window_);
    glClearColor(0.5f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program_id_);

    std::unique_ptr<const Shape> shape(new Shape(0, 2, 4, rectangleVertex));

    shape->Draw();


    glfwSwapBuffers(window_);
    //glfwWaitEvents();
    glfwPollEvents();

    return true;
}

int main(int argc, char *argv[])
{
    MyWorld my_world;
    
    /*** Initialize ***/
    /* Initialize GLFW */
    RUN_CHECK(glfwInit() == GL_TRUE);
    

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
        my_world.RunFrame();

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
