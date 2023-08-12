#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>

Camera::Camera(float x, float y, float z, float camSpeed)
{
	position = glm::vec3(x, y, z);
	speed = camSpeed;
}

//Returns the direction that the camera should go
//in after a key has been pressed or released,
//pass in the current key that is being pressed or released,
//along with the action (release or press) and also the current direction
//that the camera is moving in. Pass in the key that will change the direction
//to the target direction
MovementDirection directionFromKey(
		int currentKey, 
		int currentAction, 
		MovementDirection currentDirection,
		int key, 
		MovementDirection targetDirection)
{
	//Press the key, change to the target direction
	if(currentKey == key && currentAction == GLFW_PRESS)
		return targetDirection;
	//Release the key, check if the camera is currently moving in
	//the direction that this key controls and if it is, kill
	//movement
	else if(currentKey == key && currentAction == GLFW_RELEASE
			&& currentDirection == targetDirection)
		return NO_MOVEMENT;

	//Otherwise just return the current direction
	return currentDirection;
}

void Camera::handleKeyInput(int key, int action)
{
	//W moves forward
	movementDirection = 
		directionFromKey(key, action, movementDirection, GLFW_KEY_W, FORWARD);
	//S moves backward
	movementDirection = 
		directionFromKey(key, action, movementDirection, GLFW_KEY_S, BACKWARD);
	//A strafes left
	strafeDirection = 
		directionFromKey(key, action, strafeDirection, GLFW_KEY_A, STRAFE_LEFT);
	//D strafes right
	strafeDirection = 
		directionFromKey(key, action, strafeDirection, GLFW_KEY_D, STRAFE_RIGHT);
	//Left Shift flys down
	flyingDirection =
		directionFromKey(key, action, flyingDirection, GLFW_KEY_LEFT_SHIFT, FLY_DOWN);
	//Space flys up
	flyingDirection =
		directionFromKey(key, action, flyingDirection, GLFW_KEY_SPACE, FLY_UP);
}

void Camera::handleMouseMovement(GLFWwindow *win, float oldMousex, float oldMousey, float dt)
{
	int cursorMode = glfwGetInputMode(win, GLFW_CURSOR);
	if(cursorMode == GLFW_CURSOR_NORMAL)
		return;

	double x, y;
	glfwGetCursorPos(win, &x, &y);

	//Rotate the yaw of the camera when the cursor moves left and right
	if(x != oldMousex)
		yaw += 0.05f * dt * (x - oldMousex);

	//Rotate the pitch of the camera when the cursor moves up and down
	if(y != oldMousey)
		pitch += 0.05f * dt * (y - oldMousey);

	//Clamp the pitch to be between -pi / 2 radians and pi / 2 radians
	if(pitch < -3.14159 / 2.0f)
		pitch = -3.14159 / 2.0f;
	if(pitch > 3.14159 / 2.0f)
		pitch = 3.14159 / 2.0f;
}

void Camera::move(float dt)
{
	//Move forwards/backwards
	switch(movementDirection)
	{
	case FORWARD:
		position += glm::vec3(sin(yaw) * speed, 0.0f, -cos(yaw) * speed) * dt;
		break;
	case BACKWARD:
		position -= glm::vec3(sin(yaw) * speed, 0.0f, -cos(yaw) * speed) * dt;
		break;
	default:
		break;
	}

	//Strafing
	switch(strafeDirection)
	{
	case STRAFE_LEFT:
		position += glm::vec3(sin(-yaw - 3.14159f / 2.0f) * speed, 
							  0.0f,
							  cos(-yaw - 3.14159f / 2.0f) * speed) * dt;
		break;
	case STRAFE_RIGHT:
		position += glm::vec3(sin(-yaw + 3.14159f / 2.0f) * speed, 
							  0.0f,
							  cos(-yaw + 3.14159f / 2.0f) * speed) * dt;
		break;
	default:
		break;
	}

	//Fly
	switch(flyingDirection)
	{
	case FLY_UP:
		position += glm::vec3(0.0f, 5.0f, 0.0f) * dt;
		break;
	case FLY_DOWN:
		position -= glm::vec3(0.0f, 5.0f, 0.0f) * dt;
		break;
	default:
		break;
	}
}

//Returns the view matrix
glm::mat4 Camera::viewMatrix()
{
	return glm::rotate(glm::mat4(1.0f), pitch, glm::vec3(1.0f, 0.0f, 0.0f)) *
		   glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0.0f, 1.0f, 0.0f)) *
		   glm::translate(glm::mat4(1.0f), -position);
}
