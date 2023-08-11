#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

int main()
{
	//Try to initialize glfw
	if(!glfwInit())
	{
		std::cerr << "Failed to init glfw!\n";
		return -1;
	}

	//Try to initialize window
	GLFWwindow* win = glfwCreateWindow(800, 600, "Blockgame", NULL, NULL);
	if(!win)
	{
		std::cerr << "Failed to create window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(win);
	glfwSwapInterval(1);

	//Try to load glad
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to load glad!\n";
		glfwTerminate();
		return -1;
	}

	glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
	while(!glfwWindowShouldClose(win))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);

		glfwSwapBuffers(win);
		glfwPollEvents();
	}

	glfwTerminate();
}
