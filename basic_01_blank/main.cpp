/*** Include ***/
/* for general */
#include <stdint.h>
#include <stdio.h>
#include <fstream> 
#include <vector>
#include <string>
#include <chrono>

/* for GLFW */
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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
int main(int argc, char *argv[])
{
	/*** Initialize ***/
	GLFWwindow* window;

	/* Initialize GLFW */
	RUN_CHECK(glfwInit() == GL_TRUE);

	/* Create a window (x4 anti-aliasing, OpenGL3.3 Core Profile)*/
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	RUN_CHECK(window = glfwCreateWindow(720, 480, "main", NULL, NULL));
	glfwMakeContextCurrent(window);
	
	/* Ensure not to miss input */
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	/* Dark blue background */
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	
	/*** Start loop ***/
	while(1) {
		/* Clear the screen */
		glClear(GL_COLOR_BUFFER_BIT);

		/* Swap buffers */
		glfwSwapBuffers(window);
		glfwPollEvents();

		/* Check if the ESC key was pressed or the window was closed */
		if (glfwWindowShouldClose(window) != 0 || glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			break;
		}
	}

	/*** Finalize ***/
	/* Close OpenGL window and terminate GLFW */
	glfwTerminate();

	return 0;
}
