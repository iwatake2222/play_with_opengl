#ifndef CAMERA_CONTROL_H
#define CAMERA_CONTROL_H

void CameraControl_initialize(GLFWwindow* window, glm::vec3 position, float horizontalAngle, float verticalAngle);
glm::mat4 CameraControl_getProjectionMatrix();
glm::mat4 CameraControl_getViewMatrix();
void CameraControl_update(GLFWwindow* window);

#endif
