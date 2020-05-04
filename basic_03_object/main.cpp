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
	bool res = loadAssImp("resource/suzanne.obj", indices, vertices, uvs, normals);
	//bool res = loadOBJ("resource/suzanne.obj", vertices, uvs, normals);

	/* Create Vertex Array Object */
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	/* Create Vertex Buffer Object and copy data, then set attribute buffer */
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	GLuint uvBuffer;
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	/* Bind Texture */
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(textureId, 0);

	/* Initialize camera matrix controls (Initial position : on +Z, toward -Z) */
	CameraControls_initialize(window, glm::vec3(0, 0, 5), 3.14f, 0.0f);

	/*** Start loop ***/
	while(1) {
		/* Clear the screen */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Camera matrix */
		CameraControls_update(window);
		glm::mat4 Projection = CameraControls_getProjectionMatrix();
		glm::mat4 View = CameraControls_getViewMatrix();

		/* Model matrix */
		glm::mat4 Model = glm::mat4(1.0f);
		static float rotX = 0.0f;
		glm::mat4 myRotationAxis = glm::rotate(rotX++ / (2 * 3.14f), glm::vec3(1, 0, 0));
		Model = myRotationAxis * Model;

		/* Calculate ModelViewProjection matrix and send it to shader */
		glm::mat4 MVP = Projection * View * Model;
		glUseProgram(programId);
		glUniformMatrix4fv(matrixId, 1, GL_FALSE, &MVP[0][0]);

		/* Draw the triangle */
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
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
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

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
