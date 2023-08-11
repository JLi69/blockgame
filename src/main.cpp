#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "shader.h"
#include "block.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

void handleWindowResize(GLFWwindow *win, int newWidth, int newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
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
	glfwMakeContextCurrent(win);
	glfwSwapInterval(1);

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
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
	while(!glfwWindowShouldClose(win))
	{
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

			int perspLocation = glGetUniformLocation(program, "uPerspectiveViewMat");
			int transLocation = glGetUniformLocation(program, "uTransformMat");
			
			glUniformMatrix4fv(perspLocation, 1, GL_FALSE, glm::value_ptr(persp));
			glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(trans));
		}

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
	}

	glfwTerminate();
}
