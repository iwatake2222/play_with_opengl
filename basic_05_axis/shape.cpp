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

#include "shape.h"
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
Object::Object(GLuint position_loc, GLuint color_loc, const std::vector<Object::Vertex>& vertex_list, const std::vector<GLuint>& index_list)
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_list.size() * sizeof(Vertex), vertex_list.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<Vertex*>(0)->position);
    glEnableVertexAttribArray(position_loc);
    glVertexAttribPointer(color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<Vertex*>(0)->color);
    glEnableVertexAttribArray(color_loc);

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


Shape::Shape(const std::vector<Object::Vertex>&vertex_list, const std::vector<GLuint>&index_list)
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
    m_program_id = CreateShaderProgram(vsrc, fsrc);
    GLint position_loc = glGetAttribLocation(m_program_id, "position");
    GLint color_loc = glGetAttribLocation(m_program_id, "color");
    m_modelviewprojection_loc = glGetUniformLocation(m_program_id, "modelviewprojection");

    m_object = std::make_unique<Object>(position_loc, color_loc, vertex_list, index_list);

    m_vertex_num = static_cast<GLsizei>(vertex_list.size());
    m_index_num = static_cast<GLsizei>(index_list.size());
}

void Shape::Draw(const Matrix & viewprojection, const Matrix & model) const
{
    glUseProgram(m_program_id);
    const Matrix modelviewprojection = viewprojection * model;
    glUniformMatrix4fv(m_modelviewprojection_loc, 1, GL_TRUE, modelviewprojection.Data());
    m_object->Bind();
    Execute();
}

void Shape::Execute() const
{
    //glDrawArrays(GL_LINE_LOOP, 0, m_vertex_num);
    glDrawArrays(GL_LINES, 0, m_vertex_num);
}


void ShapeIndex::Execute() const
{
    glDrawElements(GL_LINES, m_index_num, GL_UNSIGNED_INT, 0);
}

void ShapeSolid::Execute() const
{
    glDrawArrays(GL_TRIANGLES, 0, m_vertex_num);
}

void ShapeSolidIndex::Execute() const
{
    glDrawElements(GL_TRIANGLES, m_index_num, GL_UNSIGNED_INT, 0);
}
