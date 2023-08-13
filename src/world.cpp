#include "world.hpp"
#include <glad/glad.h>

#include <iostream>

void addVertices(std::vector<float> &chunk, 
				 float vertices[],
				 float textureCoords[],
				 int32_t x,
				 int32_t y,
				 int32_t z,
				 uint8_t block);

World::World(uint32_t size, uint32_t height)
{
	blocks = new uint8_t[size * size * height];	
	worldSize = size;
	worldHeight = height;
	buffers = new unsigned int[(size / 16 + 1) * (size / 16 + 1) * 2];
	chunks = new std::vector<float>[(size / 16 + 1) * (size / 16 + 1)];
	glGenBuffers((size / 16 + 1) * (size / 16 + 1) * 2, buffers);
}

World::~World()
{
	//glDeleteBuffers((worldSize / 16 + 1) * (worldSize / 16 + 1) * 2, buffers);
	delete[] blocks;
	delete[] buffers;
	delete[] chunks;
}

void World::generateWorld()
{
	std::cerr << "Building terrain...\n";
	// Fill in the world with blocks
	for(int32_t x = -(int32_t)worldSize / 2; x < (int32_t)worldSize / 2; x++)
	{
		for(int32_t y = 0; y < (int32_t)worldHeight; y++)
		{
			for(int32_t z = -(int32_t)worldSize / 2; z < (int32_t)worldSize / 2; z++)
			{
				if(y == worldHeight / 2 - 1)
					setBlock(x, y, z, GRASS);
				else if(y < worldHeight / 2 - 1)
					setBlock(x, y, z, STONE);
				else
					setBlock(x, y, z, AIR);
			}
		}
	}
}

uint8_t World::getBlock(int32_t x, int32_t y, int32_t z)
{
	int32_t indexX = x + worldSize / 2,
			indexY = worldHeight - 1 - y,
			indexZ = z + worldSize / 2;

	if(indexX < 0 || indexY < 0 || indexZ < 0 ||
	   indexX >= worldSize || indexY >= worldHeight || indexZ >= worldSize)
		return 0;

	return blocks[indexY * worldSize * worldSize + indexZ * worldSize + indexX];
}

void World::setBlock(int32_t x, int32_t y, int32_t z, uint8_t block)
{
	int32_t indexX = x + worldSize / 2,
			indexY = worldHeight - 1 - y,
			indexZ = z + worldSize / 2;

	if(indexX < 0 || indexY < 0 || indexZ < 0 ||
	   indexX >= worldSize || indexY >= worldHeight || indexZ >= worldSize)
		return;

	blocks[indexY * worldSize * worldSize + indexZ * worldSize + indexX] = block;
}

void addVertices(std::vector<float> &chunk, 
				 float vertices[],
				 float textureCoords[],
				 int32_t x,
				 int32_t y,
				 int32_t z,
				 uint8_t blockType)
{
	int32_t chunkX = x / 16,
			chunkZ = z / 16;

	if(x < 0)
		chunkX -= 1;
	if(z < 0)
		chunkZ -= 1;

	for(int i = 0; i < 6; i++)
	{
		chunk.push_back(vertices[i * 3] + x * 2.0f);
		chunk.push_back(vertices[i * 3 + 1] + y * 2.0f);
		chunk.push_back(vertices[i * 3 + 2] + z * 2.0f);

		chunk.push_back(textureCoords[i * 2] * 1.0f / 16.0f + (blockType % 16) * 1.0f / 16.0f);
		chunk.push_back(textureCoords[i * 2 + 1] * 1.0f / 16.0f + (blockType / 16) * 1.0f / 16.0f);
	}
}

void World::addBlockVertices(int32_t x, int32_t y, int32_t z, int32_t chunkX, int32_t chunkZ)
{
	if(getBlock(x, y, z) == AIR)
		return;

	uint32_t index = (chunkX + worldSize / (2 * 16)) * (worldSize / 16 + 1) +
					 (chunkZ + worldSize / (2 * 16));

	if(getBlock(x + 1, y, z) == AIR)
	{
		//Right face	
		float rightFace[] = {
			1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, -1.0f,

			1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, 1.0f, -1.0f,
		};

		float rightFaceTexture[] = {
			0.0f, 0.0f,	
			0.0f, 1.0f,
			1.0f, 1.0f,

			0.0f, 0.0f,			
			1.0f, 1.0f,
			1.0f, 0.0f,	
		};

		addVertices(chunks[index], rightFace, rightFaceTexture, x, y, z, getBlock(x, y, z));
	}

	if(getBlock(x - 1, y, z) == AIR)
	{
		//Left face	
		float leftFace[] = {
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,

			-1.0f, 1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f, 1.0f, 1.0f,
		};

		float leftFaceTexture[] = {
			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
	
			0.0f, 0.0f,
			0.0f, 1.0f,		
			1.0f, 0.0f,	
		};


		addVertices(chunks[index], leftFace, leftFaceTexture, x, y, z, getBlock(x, y, z));
	}

	if(getBlock(x, y + 1, z) == AIR)
	{
		//Top face
		float topFace[] = {
			-1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, -1.0f,	

			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,	
		};

		float topFaceTexture[] = {
			0.0f, 1.0f,
			1.0f, 1.0f,
			0.0f, 0.0f,	

			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,	
		};

		addVertices(chunks[index], topFace, topFaceTexture, x, y, z, getBlock(x, y, z));
	}

	if(getBlock(x, y - 1, z) == AIR)
	{
		//Bottom face
		float bottomFace[] = {
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,

			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, 1.0f,	
		};

		float bottomFaceTexture[] = {
			1.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,

			1.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 0.0f,
		};

		addVertices(chunks[index], bottomFace, bottomFaceTexture, x, y, z, getBlock(x, y, z));
	}

	if(getBlock(x, y, z + 1) == AIR)
	{				
		//Front face
		float frontFace[] = {
			1.0f, 1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,

			1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,		
		};

		float frontFaceTexture[] = {
			1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,

			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
		};

		addVertices(chunks[index], frontFace, frontFaceTexture, x, y, z, getBlock(x, y, z));
	}

	if(getBlock(x, y, z - 1) == AIR)
	{
		//Back face
		float backFace[] = {
			-1.0f, -1.0f, -1.0f,
			-1.0f, 1.0f, -1.0f,
			1.0f, 1.0f, -1.0f,

			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, 1.0f, -1.0f,	
		};

		float backFaceTexture[] = {
			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,

			0.0f, 1.0f,	
			1.0f, 1.0f,
			0.0f, 0.0f,
		};	

		addVertices(chunks[index], backFace, backFaceTexture, x, y, z, getBlock(x, y, z));	
	}
}

void World::buildChunk(int32_t chunkX, int32_t chunkZ)
{
	std::cerr << "Building chunk: " << chunkX << ", " << chunkZ << '\n';

	int32_t worldChunkX = chunkX * 16,
			worldChunkZ = chunkZ * 16;

	for(int32_t x = worldChunkX; x < worldChunkX + 16; x++)
		for(int32_t y = 0; y < worldHeight; y++)
			for(int32_t z = worldChunkZ; z < worldChunkZ + 16; z++)
				addBlockVertices(x, y, z, chunkX, chunkZ);
	
	uint32_t index = ((chunkX + worldSize / (2 * 16)) * (worldSize / 16 + 1) +
					 (chunkZ + worldSize / (2 * 16)));	
}

void World::buildAllChunks()
{
	std::cerr << "Building all chunks...\n";

	for(int32_t x = -(int32_t)worldSize / (2 * 16); x < (int32_t)worldSize / (2 * 16); x++)
		for(int32_t z = -(int32_t)worldSize / (2 * 16); z < (int32_t)worldSize / (2 * 16); z++)
			buildChunk(x, z);
}

void World::displayWorld()
{
	for(int32_t x = -(int32_t)worldSize / (2 * 16); x < (int32_t)worldSize / (2 * 16); x++)
	{
		for(int32_t z = -(int32_t)worldSize / (2 * 16); z < (int32_t)worldSize / (2 * 16); z++)
		{
			uint32_t index = ((x + worldSize / (2 * 16)) * (worldSize / 16 + 1) +
					 (z + worldSize / (2 * 16)));
			glBindBuffer(GL_ARRAY_BUFFER, buffers[index * 2]);			
			glBufferData(GL_ARRAY_BUFFER, 
				 chunks[index].size() * sizeof(float), 
				 chunks[index].data(),
				 GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
			glEnableVertexAttribArray(0);	
			glBindBuffer(GL_ARRAY_BUFFER, buffers[index * 2 + 1]);	
			glBufferData(GL_ARRAY_BUFFER, 
				 chunks[index].size() * sizeof(float), 
				 chunks[index].data(),
				 GL_STATIC_DRAW);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));		
			glEnableVertexAttribArray(1);
			glDrawArrays(GL_TRIANGLES, 0, chunks[index].size() / 5);
		}
	}
}
