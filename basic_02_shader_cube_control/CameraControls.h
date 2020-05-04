#ifndef CAMERA_CONTROLS_H
#define CAMERA_CONTROLS_H

void CameraControls_initialize(GLFWwindow* window, glm::vec3 position, float horizontalAngle, float verticalAngle);
glm::mat4 CameraControls_getProjectionMatrix();
glm::mat4 CameraControls_getViewMatrix();
void CameraControls_update(GLFWwindow* window);

#endif
