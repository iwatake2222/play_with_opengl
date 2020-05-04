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

#include "CameraControls.h"

/*** Macro ***/
/* Settings */
static const float KEY_SPEED = 3.0f; // 3 units / second
static const float MOUSE_SPEED = 0.005f;

/*** Global variables ***/
/* For camera matrix */
static glm::vec3 s_position;
static float s_horizontalAngle;
static float s_verticalAngle;

/* Save calculated vectors to avoid re-calculation */
static glm::vec3 s_direction;
static glm::vec3 s_right;	
static glm::vec3 s_up;

/* Mouse status */
static double s_mouseX = -1;
static double s_mouseY = -1;

/*** Functions ***/
static void updateOrientationVectors()
{
	/* Spherical coordinates to Cartesian coordinates conversion */
	s_direction = glm::vec3(
		cos(s_verticalAngle) * sin(s_horizontalAngle),
		sin(s_verticalAngle),
		cos(s_verticalAngle) * cos(s_horizontalAngle)
	);

	s_right = glm::vec3(
		sin(s_horizontalAngle - 3.14f / 2.0f),
		0,
		cos(s_horizontalAngle - 3.14f / 2.0f)
	);

	s_up = glm::cross(s_right, s_direction);
}

static void setOrientation(float horizontalAngle, float verticalAngle)
{
	s_horizontalAngle = horizontalAngle;
	s_verticalAngle = verticalAngle;
}

static void updateOrientation(float deltaHorizontalAngle, float deltaVerticalAngle)
{
	s_horizontalAngle += deltaHorizontalAngle;
	s_verticalAngle += deltaVerticalAngle;
	updateOrientationVectors();
}

static void moveForward(float delta)
{
	s_position += s_direction * delta;
}

static void moveUp(float delta)
{
	s_position += s_up * delta;
}

static void moveRight(float delta)
{
	s_position += s_right * delta;
}

static void callbackScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	moveForward((float)yoffset);
}

void CameraControls_initialize(GLFWwindow* window, glm::vec3 position, float horizontalAngle, float verticalAngle)
{
	s_position = position;
	setOrientation(horizontalAngle, verticalAngle);
	updateOrientationVectors();

	glfwGetCursorPos(window, &s_mouseX, &s_mouseY);
	glfwSetScrollCallback(window, callbackScroll);
}

glm::mat4 CameraControls_getProjectionMatrix()
{
	glm::mat4 ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	//glm::mat4 ProjectionMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f); // In world coordinates
	return ProjectionMatrix;
}

glm::mat4 CameraControls_getViewMatrix()
{
	glm::mat4 viewMatrix = glm::lookAt(
		s_position,           // Camera is here
		s_position + s_direction, // and looks here : at the same s_position, plus "direction"
		s_up                  // Head is up (set to 0,-1,0 to look upside-down)
	);
	return viewMatrix;
}

void CameraControls_update(GLFWwindow* window)
{
	/*** Compute time difference between current and last frame ***/
	static double s_lastTime = glfwGetTime();
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - s_lastTime);

	/*** Compute mouse cursol travel distance ***/
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	float mouseMoveX = (float)(xpos - s_mouseX);
	float mouseMoveY = (float)(ypos - s_mouseY);
	s_mouseX = xpos;
	s_mouseY = ypos;

	/*** Compute direction ***/
	/* Get mouse left drag for orientation */
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		updateOrientation(MOUSE_SPEED * mouseMoveX, MOUSE_SPEED * mouseMoveY);
	}

	/* Get key for orientation */
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		updateOrientation(0, deltaTime * KEY_SPEED);
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		updateOrientation(0, -deltaTime * KEY_SPEED);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		updateOrientation(deltaTime * KEY_SPEED, 0);
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		updateOrientation(-deltaTime * KEY_SPEED, 0);
	}
	
	/*** Compute s_position ***/
	/* Get mouse right drag for position */
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		moveRight(MOUSE_SPEED * mouseMoveX);
		moveUp(MOUSE_SPEED * mouseMoveY);
	}

	/* Get key for position */
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		moveRight(-deltaTime * KEY_SPEED);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		moveUp(-deltaTime * KEY_SPEED);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		moveRight(deltaTime * KEY_SPEED);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		moveUp(deltaTime * KEY_SPEED);
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
		moveForward(deltaTime * KEY_SPEED);
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
		moveForward(-deltaTime * KEY_SPEED);
	}

	s_lastTime = currentTime;
}