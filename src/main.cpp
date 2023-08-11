#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "shader.h"
#include "block.h"
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

static glm::vec3 cameraPosition = glm::vec3(0.0f);
static glm::vec3 cameraMovement = glm::vec3(0.0f);
static float cameraPitch = 0.0f;
static float cameraYaw = 0.0f;

void handleWindowResize(GLFWwindow *win, int newWidth, int newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
}

//TODO: clean up this code
void handleKeyInput(GLFWwindow *win, int key, int scancode, int action, int mods)
{
	//Release cursor
	if(key == GLFW_KEY_ESCAPE)	
		glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	//Move forward
	if(key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		cameraMovement.x = sin(cameraYaw) * 5.0f;
		cameraMovement.z = -cos(cameraYaw) * 5.0f;
	}
	else if(key == GLFW_KEY_W && action == GLFW_RELEASE)
	{
		cameraMovement.x = 0.0f;
		cameraMovement.z = 0.0f;
	}
	//Move backwards
	if(key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		cameraMovement.x = -sin(cameraYaw) * 5.0f;
		cameraMovement.z = cos(cameraYaw) * 5.0f;
	}
	else if(key == GLFW_KEY_S && action == GLFW_RELEASE)
	{
		cameraMovement.x = 0.0f;
		cameraMovement.z = 0.0f;
	}
	//Move to the left
	if(key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		cameraMovement.x = sin(-cameraYaw - 3.14159f / 2.0f) * 5.0f;
		cameraMovement.z = cos(-cameraYaw - 3.14159f / 2.0f) * 5.0f;
	}
	else if(key == GLFW_KEY_A && action == GLFW_RELEASE)
	{
		cameraMovement.x = 0.0f;
		cameraMovement.z = 0.0f;
	}
	//Move to the right
	if(key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		cameraMovement.x = sin(-cameraYaw + 3.14159f / 2.0f) * 5.0f;
		cameraMovement.z = cos(-cameraYaw + 3.14159f / 2.0f) * 5.0f;
	}
	else if(key == GLFW_KEY_D && action == GLFW_RELEASE)
	{
		cameraMovement.x = 0.0f;
		cameraMovement.z = 0.0f;
	}
	//Move up
	if(key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		cameraMovement.y = 5.0f;		
	else if(key == GLFW_KEY_SPACE && action == GLFW_RELEASE && cameraMovement.y == 5.0f)
		cameraMovement.y = 0.0f;		
	//Move down
	if(key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
		cameraMovement.y = -5.0f;		
	else if(key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE && cameraMovement.y == -5.0f)
		cameraMovement.y = 0.0f;
}

void handleMouseInput(GLFWwindow *win, int button, int action, int mods)
{
	if(button == GLFW_MOUSE_BUTTON_LEFT)
	{
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

		{
			int width, height;
			glfwGetWindowSize(win, &width, &height);

			glm::mat4 persp = glm::perspective(
					75.0f / 180.0f * 3.14159f, (float)width / (float)height, 0.1f, 1000.0f
			);
			glm::mat4 trans = glm::translate(
				glm::mat4(1.0f), glm::vec3(0.0f, -2.5f, -8.0f)
			);

			glm::mat4 view =
				glm::rotate(glm::mat4(1.0f), cameraPitch, glm::vec3(1.0f, 0.0f, 0.0f)) *
				glm::rotate(glm::mat4(1.0f), cameraYaw, glm::vec3(0.0f, 1.0f, 0.0f)) *
				glm::translate(glm::mat4(1.0f), -cameraPosition);

			int perspLocation = glGetUniformLocation(program, "uPerspectiveViewMat");
			int transLocation = glGetUniformLocation(program, "uTransformMat");
			
			glUniformMatrix4fv(perspLocation, 1, GL_FALSE, glm::value_ptr(persp * view));
			glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(trans));
		}

		//Update camera
		cameraPosition += cameraMovement * (float)dt;

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

		double x, y;
		glfwGetCursorPos(win, &x, &y);

		if(x != mousex)
		{
			cameraYaw += 0.05f * dt * (x - mousex);
		}

		if(y != mousey)
		{
			cameraPitch += 0.05f * dt * (y - mousey);

			if(cameraPitch < -3.14159 / 2.0f)
				cameraPitch = -3.14159 / 2.0f;
			if(cameraPitch > 3.14159 / 2.0f)
				cameraPitch = 3.14159 / 2.0f;	
		}
		mousex = x;
		mousey = y;

		double end = glfwGetTime();
		dt = end - start;
	}

	glfwTerminate();
}
