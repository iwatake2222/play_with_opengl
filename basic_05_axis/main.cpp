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

#include "matrix.h"
#include "transform.h"
#include "window.h"
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
    Object(GLuint position_index, GLuint color_index, const std::vector<Object::Vertex>& vertex_list, const std::vector<GLuint>& index_list);
    virtual ~Object();
    void Bind() const;
private:
    Object(const Object& object);   // not allowed
    Object& operator=(const Object& object);    // not allowed
private:
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ibo;
};

Object::Object(GLuint position_index, GLuint color_index, const std::vector<Object::Vertex>& vertex_list, const std::vector<GLuint>& index_list)
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_list.size() * sizeof(Vertex), vertex_list.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(position_index, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<Vertex*>(0)->position);
    glEnableVertexAttribArray(position_index);
    glVertexAttribPointer(color_index, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<Vertex*>(0)->color);
    glEnableVertexAttribArray(color_index);

    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_list.size() * sizeof(GLuint), index_list.data(), GL_STATIC_DRAW);
}

Object::~Object()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ibo);
}

void Object::Bind() const
{
    glBindVertexArray(m_vao);
}

class Shape
{
public:
    Shape(const std::vector<Object::Vertex>& vertex_list, const std::vector<GLuint>& index_list);
    void Draw(const Matrix& viewprojection) const;
private:
    virtual void Execute() const;

protected:
    GLuint program_id;
    GLint position_loc;
    GLint color_loc;
    GLint modelviewprojection_loc;

    GLsizei m_vertex_num;
    GLsizei m_index_num;

private:
    std::shared_ptr<Object> object;
};

Shape::Shape(const std::vector<Object::Vertex>& vertex_list, const std::vector<GLuint>& index_list)
{
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
    program_id = CreateShaderProgram(vsrc, fsrc);
    position_loc = glGetAttribLocation(program_id, "position");
    color_loc = glGetAttribLocation(program_id, "color");
    modelviewprojection_loc = glGetUniformLocation(program_id, "modelviewprojection");

    object = std::make_unique<Object>(position_loc, color_loc, vertex_list, index_list);

    m_vertex_num = vertex_list.size();
    m_index_num = index_list.size();
}

void Shape::Draw(const Matrix& viewprojection) const
{
    glUseProgram(program_id);
    const Matrix r = Transform::Rotate(static_cast<GLfloat>(glfwGetTime()), 0.0f, 1.0f, 0.0f);
    const Matrix translation = Transform::Translate(0.0f, 0.0f, 0.0f);
    const Matrix model = translation * r;
    const Matrix modelviewprojection = viewprojection * model;

    glUniformMatrix4fv(modelviewprojection_loc, 1, GL_TRUE, modelviewprojection.Data());

    object->Bind();
    Execute();
}

void Shape::Execute() const
{
    glDrawArrays(GL_LINE_LOOP, 0, m_vertex_num);
}


class SolidShape : public Shape
{
public:
    SolidShape(const std::vector<Object::Vertex>& vertex_list);
private:
    virtual void Execute() const;
};

SolidShape::SolidShape(const std::vector<Object::Vertex>& vertex_list)
    : Shape(vertex_list, {})
{
    //
}

void SolidShape::Execute() const
{
    glDrawArrays(GL_TRIANGLES, 0, m_vertex_num);
}


//
//class ShapeIndex : public Shape
//{
//public:
//    ShapeIndex(GLuint attr_index, GLint elem_size, GLsizei vertex_num, const Object::Vertex* vertex, GLsizei index_num, const GLuint* index);
//private:
//    virtual void Execute() const;
//protected:
//    const GLsizei index_num;
//};
//
//ShapeIndex::ShapeIndex(GLuint attr_index, GLint elem_size, GLsizei vertex_num, const Object::Vertex* vertex, GLsizei index_num, const GLuint* index)
//    : Shape(attr_index, elem_size, vertex_num, vertex, index_num, index), index_num(index_num)
//{
//    //
//}
//void ShapeIndex::Execute() const
//{
//    glDrawElements(GL_LINES, index_num, GL_UNSIGNED_INT, 0);
//}
//
//
//
//
//class SolidShapeIndex : public ShapeIndex
//{
//public:
//    SolidShapeIndex(GLuint attr_index, GLint elem_size, GLsizei vertex_num, const Object::Vertex* vertex, GLsizei index_num, const GLuint* index);
//private:
//    virtual void Execute() const;
//protected:
//    const GLsizei index_num;
//};
//
//SolidShapeIndex::SolidShapeIndex(GLuint attr_index, GLint elem_size, GLsizei vertex_num, const Object::Vertex* vertex, GLsizei index_num, const GLuint* index)
//    : ShapeIndex(attr_index, elem_size, vertex_num, vertex, index_num, index), index_num(index_num)
//{
//    //
//}
//void SolidShapeIndex::Execute() const
//{
//    glDrawElements(GL_TRIANGLES, index_num, GL_UNSIGNED_INT, 0);
//}



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

const std::vector<Object::Vertex> solidCubeVertex
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
const std::vector<GLuint> solidCubeIndex 
{
0, 1, 2, 3, 4, 5, // 左
6, 7, 8, 9, 10, 11, // 裏
12, 13, 14, 15, 16, 17, // 下
18, 19, 20, 21, 22, 23, // 右
24, 25, 26, 27, 28, 29, // 上
30, 31, 32, 33, 34, 35 // 前
    }
;




int main(int argc, char *argv[])
{
    /*** Initialize ***/
    /* Initialize OpenGL */
    RUN_CHECK(glfwInit() == GL_TRUE);
    std::atexit(glfwTerminate);
    glfwSetTime(0.0);

    Window my_window;
    
    /* Create shape */
    std::unique_ptr<Shape> shape;
    //int32_t vertex_num = sizeof(rectangleVertex) / sizeof(rectangleVertex[0]);
    //int32_t vertex_elem_size = sizeof(rectangleVertex[0].position) / sizeof(rectangleVertex[0].position[0]);
    //shape = std::make_unique<Shape>(position_loc, vertex_elem_size, vertex_num, rectangleVertex);
    //int32_t vertex_num = sizeof(octahedronVertex) / sizeof(octahedronVertex[0]);
    //int32_t vertex_elem_size = sizeof(octahedronVertex[0].position) / sizeof(octahedronVertex[0].position[0]);
    //shape = std::make_unique<Shape>(position_loc, vertex_elem_size, vertex_num, octahedronVertex);
    //uint32_t vertex_num = sizeof(cubeVertex) / sizeof(cubeVertex[0]);
    //uint32_t vertex_elem_size = sizeof(cubeVertex[0].position) / sizeof(cubeVertex[0].position[0]);
    //uint32_t index_num = sizeof(wireCubeIndex) / sizeof(wireCubeIndex[0]);
    //shape = std::make_unique<ShapeIndex>(position_loc, vertex_elem_size, vertex_num, cubeVertex, index_num, wireCubeIndex);
    //uint32_t vertex_num = sizeof(solidCubeVertex) / sizeof(solidCubeVertex[0]);
    //uint32_t vertex_elem_size = sizeof(solidCubeVertex[0].position) / sizeof(solidCubeVertex[0].position[0]);
    //uint32_t index_num = sizeof(solidCubeIndex) / sizeof(solidCubeIndex[0]);
    //shape = std::make_unique<SolidShapeIndex>(position_loc, vertex_elem_size, vertex_num, solidCubeVertex, index_num, solidCubeIndex);
    shape = std::make_unique<SolidShape>(solidCubeVertex);

    /*** Start loop ***/
    while (1) {
        if (my_window.FrameStart() == false) break;

        shape->Draw(my_window.GetViewProjection());

        //const Matrix modelviewprojection2 = modelviewprojection * Transform::Translate(0.0f, 0.0f, 3.0f);
        //glUniformMatrix4fv(modelviewprojection_loc, 1, GL_TRUE, modelviewprojection2.Data());
        //shape->Draw(my_window.GetViewProjection());


        my_window.SwapBuffers();
    }

    return 0;
}
