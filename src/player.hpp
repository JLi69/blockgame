#ifndef __CAMERA_H__

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "world.hpp"
#include "hitbox.hpp"

const float JUMP_FORCE = 10.0f;
const float GRAVITY = 30.0f;

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

struct Camera
{
	glm::vec3 position = glm::vec3(0.0f);
	float pitch = 0.0f;
	float yaw = 0.0f;
	glm::mat4 viewMatrix();
};

class Player
{
	MovementDirection movementDirection;
	MovementDirection strafeDirection;
	MovementDirection flyingDirection;		

	float yvelocity = 0.0f;
	bool falling = false;
	bool jumping = false;
public:
	Hitbox hitbox = Hitbox(glm::vec3(0.0f), glm::vec3(0.0f));
	float pitch = 0.0f;
	float yaw = 0.0f;
	float speed = 0.0f;

	Player(glm::vec3 position, glm::vec3 dimensions, float camSpeed);
	void handleKeyInput(int key, int action);
	void handleMouseMovement(GLFWwindow *win, float oldMousex, float oldMousey, float dt);
	void move(float dt, World &world);
	Camera getCamera();
};

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
		MovementDirection targetDirection);

#endif

#define __CAMERA_H__
