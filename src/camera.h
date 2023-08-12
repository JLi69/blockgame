#ifndef __CAMERA_H__

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

enum MovementDirection
{
	NO_MOVEMENT,
	FORWARD,
	BACKWARD,
	STRAFE_LEFT,
	STRAFE_RIGHT,
	FLY_UP,
	FLY_DOWN
};

class Camera
{
	glm::vec3 position = glm::vec3(0.0f);
	MovementDirection movementDirection;
	MovementDirection strafeDirection;
	MovementDirection flyingDirection;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float speed = 0.0f;
public:
	Camera(float x, float y, float z, float camSpeed);
	void handleKeyInput(int key, int action);
	void handleMouseMovement(GLFWwindow *win, float oldMousex, float oldMousey, float dt);
	void move(float dt);
	glm::mat4 viewMatrix();
};

#endif

#define __CAMERA_H__
