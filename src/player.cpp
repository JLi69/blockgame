#include "player.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>
#include "hitbox.hpp"

Player::Player(glm::vec3 position, glm::vec3 dimensions, float camSpeed)
{
	hitbox = Hitbox(position, dimensions);
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

void Player::handleKeyInput(int key, int action)
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

	//Respawn
	if(key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		hitbox.position = glm::vec3(0.0f, 256.0f, 0.0f);
		yvelocity = 0.0f;
	}
}

void Player::selectBlock(int key)
{
	switch(key)
	{
	case GLFW_KEY_1:
		selectedBlock = GRASS;
		break;
	case GLFW_KEY_2:
		selectedBlock = DIRT;
		break;
	case GLFW_KEY_3:
		selectedBlock = STONE;
		break;
	case GLFW_KEY_4:
		selectedBlock = BRICK;
		break;
	case GLFW_KEY_5:
		selectedBlock = WOOD;
		break;
	}
}

void Player::handleMouseMovement(GLFWwindow *win, float oldMousex, float oldMousey, float dt)
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

void Player::move(float dt, World &world)
{
	if(!falling && jumping)
	{
		yvelocity = JUMP_FORCE;
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

	if(hitbox.position.y < -256.0f)
	{
		hitbox.position = glm::vec3(0.0f, 256.0f, 0.0f);
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

	Hitbox block;

	//Fall due to gravity
	if(falling)
	{
		velocity += glm::vec3(0.0f, yvelocity * 0.5f, 0.0f);
		yvelocity += -GRAVITY * dt;
		velocity += glm::vec3(0.0f, yvelocity * 0.5f, 0.0f);
	}

	hitbox.position.y += velocity.y * dt;
	block = searchForBlockCollision(hitbox, world);
	if(intersecting(block, hitbox) && hitbox.position.y >= block.position.y - block.dimensions.y / 2.0f)
	{
		falling = false;
		yvelocity = 0.0f;	
	}
	else if(intersecting(block, hitbox) && hitbox.position.y <= block.position.y + block.dimensions.y / 2.0f)
	{
		falling = true;
		yvelocity = -0.5f;
	}
	else
	{
		falling = true;
	}	
	hitbox = uncollideY(hitbox, block);

	hitbox.position.x += velocity.x * dt;
	block = searchForBlockCollision(hitbox, world);	
	hitbox = uncollideX(hitbox, block);

	hitbox.position.z += velocity.z * dt;	
	block = searchForBlockCollision(hitbox, world);
	hitbox = uncollideZ(hitbox, block);
}

Camera Player::getCamera()
{
	glm::vec3 offset = glm::vec3(0.0f, 0.3f, 0.0f);
	return {
		.position = hitbox.position * WORLD_SCALE + offset,
		.pitch = pitch,
		.yaw = yaw
	};
}

//Returns the view matrix
glm::mat4 Camera::viewMatrix()
{
	return glm::rotate(glm::mat4(1.0f), pitch, glm::vec3(1.0f, 0.0f, 0.0f)) *
		   glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0.0f, 1.0f, 0.0f)) *
		   glm::translate(glm::mat4(1.0f), -position);
}
