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
    
    /* Create shape */
    std::unique_ptr<Shape> cube0(new ShapeIndex(CubeWireVertex, CubeWireIndex));
    std::unique_ptr<Shape> cube1(new ShapeSolid(CubeTriangleVertex));

    /*** Start loop ***/
    while (1) {
        if (my_window.FrameStart() == false) break;

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
