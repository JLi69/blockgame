#include "block.hpp"
#include <glad/glad.h>

void createBlockMesh(unsigned int buffid)
{
	//For now, we will just use a square
	const float vertices[] = {
		//Front face
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,

		//Back face	
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,

		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,

		//Left face	
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,

		//Right face
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,

		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,

		//Top face
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,	

		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		//Bottom face
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,	
	};

	glBindBuffer(GL_ARRAY_BUFFER, buffid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	
}

void createBlockTextureCoords(unsigned int buffid)
{
	//For now, we will just use a square
	const float vertices[] = {
		//Front face
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,

		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,

		//Back face	
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		0.0f, 1.0f,	
		1.0f, 1.0f,
		0.0f, 0.0f,

		//Left face	
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
	
		0.0f, 0.0f,
		0.0f, 1.0f,		
		1.0f, 0.0f,

		//Right face
		0.0f, 0.0f,	
		0.0f, 1.0f,
		1.0f, 1.0f,

		0.0f, 0.0f,			
		1.0f, 1.0f,
		1.0f, 0.0f,

		//Top face
		0.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,	

		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		//Bottom face
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,	
	};

	glBindBuffer(GL_ARRAY_BUFFER, buffid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	
}
