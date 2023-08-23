#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "shader.hpp"
#include "player.hpp"
#include "world.hpp"
#include "hitbox.hpp"

const float FOV = 75.0f / 180.0f * 3.14159f; //In radians
const float ZNEAR = 0.1f;
const float ZFAR = 1024.0f;
const int WORLD_SIZE = 256;
const uint32_t RENDER_DIST = WORLD_SIZE / (CHUNK_SIZE * 2);

struct State
{
	Player player;
	glm::mat4 persp;
	World world;
	glm::vec3 selected;
};

float windowAspectRatio(GLFWwindow *win)
{
	int w, h;
	glfwGetWindowSize(win, &w, &h);

	if(h == 0)
		return 0;
	
	return (float)w / (float)h;
}

void handleWindowResize(GLFWwindow *win, int newWidth, int newHeight)
{
	glViewport(0, 0, newWidth, newHeight);	
	State *state = (State*)glfwGetWindowUserPointer(win);
	
	if(newHeight == 0)
		return;

	state->persp = 
		glm::perspective(FOV, (float)newWidth / (float)newHeight, ZNEAR, ZFAR);
}

void handleKeyInput(GLFWwindow *win, int key, int scancode, int action, int mods) {
	//Release cursor
	if(key == GLFW_KEY_ESCAPE)	
		glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	State *state = (State*)glfwGetWindowUserPointer(win);
	state->player.handleKeyInput(key, action);
	state->player.selectBlock(key);

	//Output player position
	if(key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		std::cerr << "Player position: " << 
					 state->player.hitbox.position.x << ", " <<
					 state->player.hitbox.position.y << ", " <<
					 state->player.hitbox.position.z << '\n';
	}
};

void handleMouseInput(GLFWwindow *win, int button, int action, int mods)
{
	int cursorMode = glfwGetInputMode(win, GLFW_CURSOR);

	if(button == GLFW_MOUSE_BUTTON_LEFT && cursorMode == GLFW_CURSOR_NORMAL) { 
		//If mouse position is inside the window,
		//and the user left clicks,
		//grab control of the mouse cursor
		double x, y;
		glfwGetCursorPos(win, &x, &y);
		int w, h;
		glfwGetWindowSize(win, &w, &h);
		if(x < (double)w && y < (double)h && x > 0.0 && y > 0.0)
			glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	State *state = (State*)glfwGetWindowUserPointer(win);

	//Destroy block
	if(button == GLFW_MOUSE_BUTTON_LEFT && 
	   action == GLFW_PRESS && 
	   cursorMode == GLFW_CURSOR_DISABLED)
	{
		glm::vec3 pos = raycast(
			state->world, 
			state->player.hitbox.position + glm::vec3(0.5f, 0.2f, 0.5f),
			state->player.yaw,
			state->player.pitch,
			5.0f
		);

		int32_t x = (int32_t)floorf(pos.x),
				y = (int32_t)floorf(pos.y),
				z = (int32_t)floorf(pos.z);

		int32_t chunkX = x / CHUNK_SIZE,
				chunkZ = z / CHUNK_SIZE;
		if(z < 0 && z % CHUNK_SIZE != 0)
			chunkZ--;
		if(x < 0 && x % CHUNK_SIZE != 0)
			chunkX--;

		state->world.setBlock(x, y, z, AIR);
		state->world.buildChunk(chunkX, chunkZ);

		if(x % CHUNK_SIZE == 0 || (x - 1) % CHUNK_SIZE == 0)
			state->world.buildChunk(chunkX - 1, chunkZ);	
		if((x + 1) % CHUNK_SIZE == 0 || x % CHUNK_SIZE == 0)
			state->world.buildChunk(chunkX + 1, chunkZ);
		if((z - 1) % CHUNK_SIZE == 0 || z % CHUNK_SIZE == 0)
			state->world.buildChunk(chunkX, chunkZ - 1);	
		if((z + 1) % CHUNK_SIZE == 0 || z % CHUNK_SIZE == 0)
			state->world.buildChunk(chunkX, chunkZ + 1);	
	}

	//Place block
	if(button == GLFW_MOUSE_BUTTON_RIGHT &&
	   action == GLFW_PRESS &&
	   cursorMode == GLFW_CURSOR_DISABLED)
	{
		glm::vec3 pos = raycast(
			state->world, 
			state->player.hitbox.position + glm::vec3(0.5f, 0.2f, 0.5f),
			state->player.yaw,
			state->player.pitch,
			5.0f
		);

		int32_t x = (int32_t)floorf(pos.x),
				y = (int32_t)floorf(pos.y),
				z = (int32_t)floorf(pos.z);

		if(state->world.getBlock(x, y, z) == AIR)
			return;

		int32_t selectedX = x,
				selectedY = y,
				selectedZ = z;

		pos -= glm::vec3(0.05f * cosf(state->player.pitch) * sinf(state->player.yaw),
						 0.05f * sinf(-state->player.pitch),
						 0.05f * cosf(state->player.pitch) * -cosf(state->player.yaw));

		x = (int32_t)floorf(pos.x);	
		y = (int32_t)floorf(pos.y);
		z = (int32_t)floorf(pos.z);
		
		if(state->world.getBlock(x, y, z) != AIR)
			return;

		Hitbox block = Hitbox(glm::vec3(x, y + 0.5f, z), glm::vec3(1.0f));	
		if(intersecting(block, state->player.hitbox))
			return;

		int32_t chunkX = x / CHUNK_SIZE,
				chunkZ = z / CHUNK_SIZE;
		if(z < 0 && z % CHUNK_SIZE != 0)
			chunkZ--;
		if(x < 0 && x % CHUNK_SIZE != 0)
			chunkX--;

		state->world.setBlock(x, y, z, state->player.selectedBlock);
		state->world.buildChunk(chunkX, chunkZ);

		if(x % CHUNK_SIZE == 0 || (x - 1) % CHUNK_SIZE == 0)
			state->world.buildChunk(chunkX - 1, chunkZ);	
		if((x + 1) % CHUNK_SIZE == 0 || x % CHUNK_SIZE == 0)
			state->world.buildChunk(chunkX + 1, chunkZ);
		if((z - 1) % CHUNK_SIZE == 0 || z % CHUNK_SIZE == 0)
			state->world.buildChunk(chunkX, chunkZ - 1);	
		if((z + 1) % CHUNK_SIZE == 0 || z % CHUNK_SIZE == 0)
			state->world.buildChunk(chunkX, chunkZ + 1);
	}
}

int main()
{			
	//Try to initialize glfw
	if(!glfwInit())
	{
		std::cerr << "Failed to init glfw!\n";
		return -1;
	}

	//Try to initialize window
	GLFWwindow *win = glfwCreateWindow(800, 600, "Blockgame", NULL, NULL);
	if(!win)
	{
		std::cerr << "Failed to create window\n";
		glfwTerminate();
		return -1;
	}
	glfwSetWindowSizeCallback(win, handleWindowResize);
	glfwSetKeyCallback(win, handleKeyInput);
	glfwSetMouseButtonCallback(win, handleMouseInput);
	glfwMakeContextCurrent(win);
	glfwSwapInterval(1);
	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Try to load glad
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to load glad!\n";
		glfwTerminate();
		return -1;
	}

	glm::vec3 playerDimensions = glm::vec3(0.6f, 1.8f, 0.6f);
	glm::vec3 playerPosition = glm::vec3(0.0f, 128.0f, 0.0f);

	State gameState = {
		.player = Player(playerPosition, playerDimensions, 4.0f),
		.persp = glm::perspective(FOV, windowAspectRatio(win), ZNEAR, ZFAR),
		.world = World(WORLD_SIZE, 128),
		.selected = glm::vec3(0.0f, 0.0f, 0.0f)
	};
	glfwSetWindowUserPointer(win, &gameState);

	//Create shaders
	ShaderProgram program(
		createShader("assets/shaders/vert.glsl", GL_VERTEX_SHADER),
		createShader("assets/shaders/frag.glsl", GL_FRAGMENT_SHADER)
	);
	program.use();

	//Texture
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	int width, height, channels;
	unsigned char *data = 
		stbi_load("assets/textures/textures.png", &width, &height, &channels, 0);
	if(data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);	
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cerr << "Failed to open textures\n";
	}
	stbi_image_free(data);

	//Main loop
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
	
	//Amount of time that passes each frame
	double dt = 0.0;
	//Mouse position
	double mousex = 0.0, mousey = 0.0;
	glfwGetCursorPos(win, &mousex, &mousey);

	//Create world
	gameState.world.generateWorld();
	gameState.world.buildAllChunks();		

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	int framesDrawn = 0;
	double frameTimer = 0.0;

	while(!glfwWindowShouldClose(win))
	{
		double start = glfwGetTime();

		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		glm::mat4 trans = glm::mat4(1.0f);	
		glm::mat4 view = gameState.player.getCamera().viewMatrix();
		glUniformMatrix4fv(program.getUniformLocation("uPerspectiveViewMat"), 1, GL_FALSE, glm::value_ptr(gameState.persp * view));
		glUniformMatrix4fv(program.getUniformLocation("uTransformMat"), 1, GL_FALSE, glm::value_ptr(trans));

		Frustum viewFrustum = createFrustumFromCamera(
			gameState.player.getCamera(),
			FOV,
			ZNEAR,
			ZFAR,
			windowAspectRatio(win)
		);

		int triCount = gameState.world.displayWorld(
			viewFrustum, 
			gameState.player.getCamera().position, 
			RENDER_DIST
		);

		//Update camera
		gameState.player.move((float)dt, gameState.world);

		{
			glm::vec3 pos = raycast(
				gameState.world, 
				gameState.player.hitbox.position + glm::vec3(0.5f, 0.2f, 0.5f),
				gameState.player.yaw,
				gameState.player.pitch,
				5.0f
			);

			gameState.selected = glm::vec3(floorf(pos.x), floorf(pos.y), floorf(pos.z));
			if(gameState.world.getBlock(
					(int32_t)gameState.selected.x, 
					(int32_t)gameState.selected.y,
					(int32_t)gameState.selected.z) == AIR)
			{
				gameState.selected.y = -999.0f;
			}

			glUniform3f(program.getUniformLocation("uSelected"), 
						gameState.selected.x, 
						gameState.selected.y,
						gameState.selected.z);
		}

		//Output OpenGL errors
		GLenum err = glGetError();
		while(err != GL_NO_ERROR)
		{
			std::cerr << "OpenGL error: " << err << '\n';
			err = glGetError();	
		}

		glfwSwapBuffers(win);
		glfwPollEvents();

		gameState.player.handleMouseMovement(win, mousex, mousey, dt);

		double x, y;
		glfwGetCursorPos(win, &x, &y);
		mousex = x;
		mousey = y;

		double end = glfwGetTime();
		dt = end - start;

		frameTimer += dt;
		if(frameTimer > 1.0)
		{
			std::cerr << "FPS: " << framesDrawn 
					  << " |  Triangles drawn: (" << triCount << ") \n";
			framesDrawn = 0;
			frameTimer = 0.0;
		}
		framesDrawn++;
	}

	gameState.world.deleteBuffers();
	glfwTerminate();
}
