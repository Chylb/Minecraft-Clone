#include "Camera.h"

Camera::Camera(glm::vec3 position, float yaw, float pitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
	this->position = position;
	this->yaw = yaw;
	this->pitch = pitch;
	updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(position, position + facingDirection, up);
}

void Camera::ProcessMovement(CameraMovement direction, float deltaTime)
{
	float velocity = (movementSpeed * (1 + 9 * sprint)) * deltaTime;
	if (direction == CameraMovement::forward)
		position += front * velocity;
	if (direction == CameraMovement::backward)
		position -= front * velocity;
	if (direction == CameraMovement::left)
		position -= right * velocity;
	if (direction == CameraMovement::right)
		position += right * velocity;
	if (direction == CameraMovement::up)
		position += up * velocity;
	if (direction == CameraMovement::down)
		position -= up * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (constrainPitch) {
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}

	updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
	zoom -= (float)yoffset;
	if (zoom < 1.0f)
		zoom = 1.0f;
	if (zoom > 45.0f)
		zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
	facingDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	facingDirection.y = sin(glm::radians(pitch));
	facingDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	facingDirection = glm::normalize(facingDirection);
	front.x = facingDirection.x;
	front.z = facingDirection.z;
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, up));
}
