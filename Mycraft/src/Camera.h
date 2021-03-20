#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

enum class CameraMovement
{
	forward,
	backward,
	left,
	right,
	up,
	down
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 20.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 facingDirection;
	glm::vec3 right;
	const inline static glm::vec3 up{ 0,1,0 };

	float yaw;
	float pitch;

	float movementSpeed;
	float mouseSensitivity;
	float zoom;

	bool sprint;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
	glm::mat4 GetViewMatrix();
	void ProcessMovement(CameraMovement direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
	void ProcessMouseScroll(float yoffset);

private:
	void updateCameraVectors();
};
