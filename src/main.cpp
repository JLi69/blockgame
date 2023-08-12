#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "block.h"
#include "camera.h"

struct State
{
	Camera camera;
	glm::mat4 persp;
};

void handleWindowResize(GLFWwindow *win, int newWidth, int newHeight)
{
	glViewport(0, 0, newWidth, newHeight);	
	State *state = (State*)glfwGetWindowUserPointer(win);
	(*state).persp = 
		glm::perspective(75.0f / 180.0f * 3.14159f, (float)newWidth / (float)newHeight, 0.1f, 1000.0f);
}

void handleKeyInput(GLFWwindow *win, int key, int scancode, int action, int mods) {
	//Release cursor
	if(key == GLFW_KEY_ESCAPE)	
		glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	State *state = (State*)glfwGetWindowUserPointer(win);
	(*state).camera.handleKeyInput(key, action);
};

void handleMouseInput(GLFWwindow *win, int button, int action, int mods)
{
	if(button == GLFW_MOUSE_BUTTON_LEFT)
	{
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
}

int main()
{	
	State gameState = {
		.camera = Camera(0.0f, 0.0f, 0.0f, 5.0f),
		.persp = glm::perspective(75.0f / 180.0f * 3.14159f, 800.0f / 600.0f, 0.1f, 1000.0f)
	};	

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
	glfwSetWindowUserPointer(win, &gameState);
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

	//Create shaders
	unsigned int program = createProgram(
		createShader("assets/shaders/vert.glsl", GL_VERTEX_SHADER),
		createShader("assets/shaders/frag.glsl", GL_FRAGMENT_SHADER)
	);
	glUseProgram(program);

	unsigned int blockMesh = createBlockMesh();
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);
	glEnableVertexAttribArray(0);	

	//Main loop
	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
	double dt = 0.0;
	double mousex = 0.0, mousey = 0.0;
	glfwGetCursorPos(win, &mousex, &mousey);
	while(!glfwWindowShouldClose(win))
	{
		double start = glfwGetTime();

		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		//TEST CUBE
		//TODO: DELETE THIS CODE LATER
		{	
			glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.5f, -8.0f));
			glm::mat4 view = gameState.camera.viewMatrix();

			int perspLocation = glGetUniformLocation(program, "uPerspectiveViewMat");
			int transLocation = glGetUniformLocation(program, "uTransformMat");	
			glUniformMatrix4fv(perspLocation, 1, GL_FALSE, glm::value_ptr(gameState.persp * view));
			glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(trans));
		}

		//Update camera
		gameState.camera.move((float)dt);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Output OpenGL errors
		GLenum err = glGetError();
		while(err != GL_NO_ERROR)
		{
			std::cerr << "OpenGL error: " << err << '\n';
			err = glGetError();	
		}

		glfwSwapBuffers(win);
		glfwPollEvents();

		gameState.camera.handleMouseMovement(win, mousex, mousey, dt);

		double x, y;
		glfwGetCursorPos(win, &x, &y);
		mousex = x;
		mousey = y;

		double end = glfwGetTime();
		dt = end - start;
	}

	glfwTerminate();
}
