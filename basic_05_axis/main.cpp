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

/* for GLFW */
#include <GL/glew.h>     /* this must be before including glfw*/
#include <GLFW/glfw3.h>

#include "matrix.h"
#include "transform.h"
#include "window.h"
#include "shape.h"
#include "object_data.h"

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
int main(int argc, char *argv[])
{
    /*** Initialize ***/
    /* Initialize OpenGL */
    RUN_CHECK(glfwInit() == GL_TRUE);
    std::atexit(glfwTerminate);
    glfwSetTime(0.0);

    Window my_window;
    my_window.LookAt(Matrix(3, 1, { 2.0f, 2.0f, 10.0f }), Matrix(3, 1, { 0.0f, 0.0f, 0.0f }), Matrix(3, 1, { 0.0f, 1.0f, 0.0f }));
    
    /* Create shape */
    std::unique_ptr<Shape> cube0(new ShapeIndex(CubeWireVertex, CubeWireIndex));
    std::unique_ptr<Shape> cube1(new ShapeSolid(CubeTriangleVertex));
    std::unique_ptr<Shape> ground(CreateGround(10, 0.1f));

    /*** Start loop ***/
    while (1) {
        if (my_window.FrameStart() == false) break;
        
        ground->Draw(my_window.GetViewProjection(), Matrix::Identity(4));

        const Matrix r = Transform::Rotate(static_cast<GLfloat>(glfwGetTime()), 0.0f, 1.0f, 0.0f);
        const Matrix translation0 = Transform::Translate(0.0f, 0.0f, 0.0f);
        const Matrix model0 = translation0 * r;
        cube0->Draw(my_window.GetViewProjection(), model0);
        const Matrix translation1 = Transform::Translate(3.0f, 0.0f, 0.0f);
        const Matrix model1 = translation1 * r;
        cube1->Draw(my_window.GetViewProjection(), model1);
        

        my_window.SwapBuffers();
    }

    return 0;
}
