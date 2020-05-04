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
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp> 

#include "shader.h"
#include "texture.h"
#include "objloader.h"
#include "CameraControls.h"
#include "Background.h"

/*** Macro ***/
/* macro functions */
#define RUN_CHECK(x)                                         \
  if (!(x)) {                                                \
    fprintf(stderr, "Error at %s:%d\n", __FILE__, __LINE__); \
    exit(1);                                                 \
  }

/* Settings */
#define WINDOW_WIDTH  720
#define WINDOW_HEIGHT 480

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
	RUN_CHECK(window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "main", NULL, NULL));
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

	/* Load shader and get handle */
	GLuint programId = LoadShaders("resource/TransformVertexShader.vertexshader", "resource/TextureFragmentShader.fragmentshader");
	GLuint matrixId = glGetUniformLocation(programId, "MVP");
	GLuint textureId = glGetUniformLocation(programId, "myTextureSampler");

	/* Read the texture */
	GLuint texture = loadDDS("resource/uvmap.DDS");

	/* Read .obj file */
	std::vector<unsigned short> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	RUN_CHECK(loadAssImp("resource/cube.obj", indices, vertices, uvs, normals));
	//RUN_CHECK(loadOBJ("resource/suzanne.obj", vertices, uvs, normals));

	/* Create Vertex Array Object */
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	/* Initialize for background */
	BackgroundDrawer_init(WINDOW_WIDTH, WINDOW_HEIGHT);

	/* Create Vertex Buffer Object and copy data */
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint uvBuffer;
	if (uvs.size() > 0) {
		glGenBuffers(1, &uvBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	}

	/* Initialize camera matrix controls (Initial position : on +Z, toward -Z) */
	CameraControls_initialize(window, glm::vec3(0, 0, 5), 3.14f, 0.0f);

	/*** Start loop ***/
	while (1) {
		/* Clear the screen */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Draw background */
		uint8_t *background = new unsigned char[WINDOW_WIDTH * WINDOW_HEIGHT * 3];
		static uint8_t s_color = 0;	 s_color++;
		for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++) {
			background[i * 3 + 0] = s_color;
			background[i * 3 + 1] = 255 - s_color;
			background[i * 3 + 2] = 0;
		}
		BackgroundDrawer_draw(WINDOW_WIDTH, WINDOW_HEIGHT, background);
		delete[] background;
		glClear(GL_DEPTH_BUFFER_BIT);		// draw background as back

		glUseProgram(programId);
		/* Camera matrix */
		CameraControls_update(window);
		glm::mat4 Projection = CameraControls_getProjectionMatrix();
		glm::mat4 View = CameraControls_getViewMatrix();

		/* Model matrix */
		glm::mat4 Model = glm::mat4(1.0f);
		static float rotY = 0.0f;
		glm::mat4 myRotationAxis = glm::rotate(rotY++ / (2 * 3.14f), glm::vec3(0, 1, 0));
		Model = myRotationAxis * Model;

		/* Calculate ModelViewProjection matrix and send it to shader */
		glm::mat4 MVP = Projection * View * Model;
		glUniformMatrix4fv(matrixId, 1, GL_FALSE, &MVP[0][0]);

		/* Bind Texture */
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(textureId, 0);

		/* Set attribute buffer */
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		/* Draw the triangle */
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
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
	BackgroundDrawer_finalize();
	/* Cleanup VBO and shader */
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &uvBuffer);
	glDeleteTextures(1, &textureId);
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(programId);

	/* Close OpenGL window and terminate GLFW */
	glfwTerminate();

	return 0;
}
