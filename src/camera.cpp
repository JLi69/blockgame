#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>
#include "hitbox.hpp"

#include <iostream>

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
	/*flyingDirection =
		directionFromKey(key, action, flyingDirection, GLFW_KEY_LEFT_SHIFT, FLY_DOWN);
	//Space flys up
	flyingDirection =
		directionFromKey(key, action, flyingDirection, GLFW_KEY_SPACE, FLY_UP);*/

	if(key == GLFW_KEY_SPACE && action == GLFW_PRESS)	
		jumping = true;
	else if(key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
		jumping = false;

	if(key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		position = glm::vec3(0.0f, 256.0f, 0.0f);
		yvelocity = 0.0f;
	}
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

void Camera::move(float dt, World &world)
{
	if(!falling && jumping)
	{
		yvelocity = 12.0f;
		falling = true;
	}

	glm::vec3 velocity = glm::vec3(0.0f);
	//Move forwards/backwards
	switch(movementDirection)
	{
	case FORWARD:
		velocity += glm::vec3(sinf(yaw) * speed, 0.0f, -cosf(yaw) * speed);
		break;
	case BACKWARD:
		velocity -= glm::vec3(sinf(yaw) * speed, 0.0f, -cosf(yaw) * speed);
		break;
	default:
		break;
	}

	//Strafing
	switch(strafeDirection)
	{
	case STRAFE_LEFT:
		velocity += glm::vec3(sinf(-yaw - 3.14159f / 2.0f) * speed, 
							  0.0f,
							  cosf(-yaw - 3.14159f / 2.0f) * speed);
		break;
	case STRAFE_RIGHT:
		velocity += glm::vec3(sinf(-yaw + 3.14159f / 2.0f) * speed, 
							  0.0f,
							  cosf(-yaw + 3.14159f / 2.0f) * speed);
		break;
	default:
		break;
	}

	if(position.y < -256.0f)
	{
		position = glm::vec3(0.0f, 256.0f, 0.0f);
		yvelocity = 0.0f;	
	}

	//Fly
	/*switch(flyingDirection)
	{
	case FLY_UP:
		position += glm::vec3(0.0f, speed, 0.0f) * dt;
		break;
	case FLY_DOWN:
		position -= glm::vec3(0.0f, speed, 0.0f) * dt;
		break;
	default:
		break;
	}*/

	Hitbox cameraHitbox;
	Hitbox block;
	glm::vec3 hitboxOffset = glm::vec3(0.0f, 0.3f, 0.0f);
	glm::vec3 hitboxDimensions = glm::vec3(0.6f, 1.8f, 0.6f);

	//Fall due to gravity
	if(falling)
	{
		velocity += glm::vec3(0.0f, yvelocity * 0.5f, 0.0f);
		yvelocity += -20.0f * dt;
		velocity += glm::vec3(0.0f, yvelocity * 0.5f, 0.0f);
	}
	position.y += velocity.y * dt;
	cameraHitbox = Hitbox(position / 2.0f - hitboxOffset, hitboxDimensions);	
	block = searchForBlockCollision(cameraHitbox, world);
	if(intersecting(block, cameraHitbox) && cameraHitbox.position.y >= block.position.y - block.dimensions.y / 2.0f)
	{
		falling = false;
		yvelocity = 0.0f;	
	}
	else if(intersecting(block, cameraHitbox) && cameraHitbox.position.y <= block.position.y + block.dimensions.y / 2.0f)
	{
		falling = true;
		yvelocity = -0.5f;
	}
	else
	{
		falling = true;
	}	
	cameraHitbox = uncollideY(cameraHitbox, block);
	position = (cameraHitbox.position + hitboxOffset) * 2.0f;

	position.x += velocity.x * dt;
	cameraHitbox = Hitbox(position / 2.0f - hitboxOffset, hitboxDimensions);
	block = searchForBlockCollision(cameraHitbox, world);	
	cameraHitbox = uncollideX(cameraHitbox, block);
	position = (cameraHitbox.position + hitboxOffset) * 2.0f;	

	position.z += velocity.z * dt;	
	cameraHitbox = Hitbox(position / 2.0f - hitboxOffset, hitboxDimensions);
	block = searchForBlockCollision(cameraHitbox, world);
	cameraHitbox = uncollideZ(cameraHitbox, block);
	position = (cameraHitbox.position + hitboxOffset) * 2.0f;	
}

//Returns the view matrix
glm::mat4 Camera::viewMatrix()
{
	return glm::rotate(glm::mat4(1.0f), pitch, glm::vec3(1.0f, 0.0f, 0.0f)) *
		   glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0.0f, 1.0f, 0.0f)) *
		   glm::translate(glm::mat4(1.0f), -position);
}
