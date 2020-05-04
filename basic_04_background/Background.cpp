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
#include "Background.h"

/*** Macro ***/
/* Settings */

/*** Global variables ***/
static GLuint s_shaderId;
static GLuint s_textureUniformId;
static GLuint s_textureId;
static GLuint s_vertexBuffer;
static GLuint s_uvBuffer;

/*** Functions ***/
void BackgroundDrawer_init(int width, int height)
{
	/* Load shader and get handle */
	s_shaderId = LoadShaders("resource/BackgroundVertexShader.vertexshader", "resource/BackgroundVertexShader.fragmentshader");
	s_textureUniformId = glGetUniformLocation(s_shaderId, "myTextureSampler");

	/* Create Texture object (blank) */
	glGenTextures(1, &s_textureId);

	/* Create Vertex Buffer Object and copy data (values are fixed) */
	static const GLfloat vertexBufferFullScreen[] = {
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		 1.0f,  1.0f,

		-1.0f, -1.0f,
		 1.0f,  1.0f,
		-1.0f,  1.0f,

	};

	static const GLfloat uvBufferFullScreen[] = {
		0.0f,  0.0f,
		0.0f,  1.0f,
		1.0f,  0.0f,

		1.0f,  0.0f,
		1.0f,  1.0f,
		0.0f,  1.0f,
	};

	glGenBuffers(1, &s_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, s_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferFullScreen), &vertexBufferFullScreen[0], GL_STATIC_DRAW);

	glGenBuffers(1, &s_uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, s_uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvBufferFullScreen), &uvBufferFullScreen[0], GL_STATIC_DRAW);
}

void BackgroundDrawer_finalize()
{
	glDeleteBuffers(1, &s_vertexBuffer);
	glDeleteBuffers(1, &s_uvBuffer);
	glDeleteTextures(1, &s_textureUniformId);
	glDeleteTextures(1, &s_textureId);
	glDeleteProgram(s_shaderId);
}

void BackgroundDrawer_draw(int width, int height, uint8_t *data)
{
	/* Update texture image for background */
	glBindTexture(GL_TEXTURE_2D, s_textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glGenerateMipmap(GL_TEXTURE_2D);

	glUseProgram(s_shaderId);

	/* Bind Texture */
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, s_textureId);
	glUniform1i(s_textureUniformId, 0);

	/* Set attribute buffer */
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, s_vertexBuffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, s_uvBuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	/* Draw the two-triangle (rectangle) as background*/
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glUseProgram(0);
}
