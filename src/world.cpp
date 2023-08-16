#include "world.hpp"
#include <glad/glad.h>
#include <math.h>
#include <iostream>

World::World(uint32_t size, uint32_t height)
{
	blocks = new uint8_t[size * size * height];	
	worldSize = size;
	worldHeight = height;

	buffers = new unsigned int[(size / CHUNK_SIZE + 1) * (size / CHUNK_SIZE + 1) * 2];	
	chunkVertexCount = new unsigned int[(size / CHUNK_SIZE + 1) * (size / CHUNK_SIZE + 1)];

	glGenBuffers((size / CHUNK_SIZE + 1) * (size / CHUNK_SIZE + 1) * 2, buffers);
}

World::~World()
{
	//glDeleteBuffers((worldSize / 16 + 1) * (worldSize / 16 + 1) * 2, buffers);
	delete[] blocks;
	delete[] buffers;
	delete[] chunkVertexCount;
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
				else if(y < worldHeight / 2 - 1 && y >= worldHeight / 2 - 4)
					setBlock(x, y, z, DIRT);	
				else if(y < worldHeight / 2 - 4)
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
				 const float vertices[],
				 const float textureCoords[],
				 int32_t x,
				 int32_t y,
				 int32_t z,
				 uint8_t blockType)
{
	for(int i = 0; i < 6; i++)
	{
		chunk.push_back(vertices[i * 3] + x * 2.0f);
		chunk.push_back(vertices[i * 3 + 1] + y * 2.0f);
		chunk.push_back(vertices[i * 3 + 2] + z * 2.0f);

		chunk.push_back(textureCoords[i * 2] * 1.0f / (float)CHUNK_SIZE + (blockType % CHUNK_SIZE) * 1.0f / (float)CHUNK_SIZE);
		chunk.push_back(textureCoords[i * 2 + 1] * 1.0f / (float)CHUNK_SIZE + (blockType / CHUNK_SIZE) * 1.0f / (float)CHUNK_SIZE);
	}
}

void World::addBlockVertices(std::vector<float> &chunk, int32_t x, int32_t y, int32_t z)
{
	if(getBlock(x, y, z) == AIR)
		return;

	if(getBlock(x + 1, y, z) == AIR)
	{
		//Right face	
		const float rightFace[] = {
			WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f,
			WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f,
			WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f,

			WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f,
			WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f,
			WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f,
		};

		const float rightFaceTexture[] = {
			0.0f, 0.0f,	
			0.0f, 1.0f,
			1.0f, 1.0f,

			0.0f, 0.0f,			
			1.0f, 1.0f,
			1.0f, 0.0f,	
		};

		addVertices(chunk, rightFace, rightFaceTexture, x, y, z, getBlock(x, y, z));
	}

	if(getBlock(x - 1, y, z) == AIR)
	{
		//Left face	
		const float leftFace[] = {
			-WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f,
			-WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f,
			-WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f,

			-WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f,
			-WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f,
			-WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f,
		};

		const float leftFaceTexture[] = {
			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
	
			0.0f, 0.0f,
			0.0f, 1.0f,		
			1.0f, 0.0f,	
		};


		addVertices(chunk, leftFace, leftFaceTexture, x, y, z, getBlock(x, y, z));
	}

	if(getBlock(x, y + 1, z) == AIR)
	{
		//Top face
		const float topFace[] = {
			-WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f,
			WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f,
			-WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f,	

			WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f,
			WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f,
			-WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f,	
		};

		const float topFaceTexture[] = {
			0.0f, 1.0f,
			1.0f, 1.0f,
			0.0f, 0.0f,	

			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,	
		};

		addVertices(chunk, topFace, topFaceTexture, x, y, z, getBlock(x, y, z));
	}

	if(getBlock(x, y - 1, z) == AIR)
	{
		//Bottom face
		const float bottomFace[] = {
			-WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f,
			WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f,
			-WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f,

			-WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f,
			WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f,
			WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f,	
		};

		const float bottomFaceTexture[] = {
			1.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f,

			1.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 0.0f,
		};

		addVertices(chunk, bottomFace, bottomFaceTexture, x, y, z, getBlock(x, y, z));
	}

	if(getBlock(x, y, z + 1) == AIR)
	{				
		//Front face
		const float frontFace[] = {
			WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f,
			-WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f,
			-WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f,

			WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f,
			-WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f,
			WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f,		
		};

		const float frontFaceTexture[] = {
			1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,

			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
		};

		addVertices(chunk, frontFace, frontFaceTexture, x, y, z, getBlock(x, y, z));
	}

	if(getBlock(x, y, z - 1) == AIR)
	{
		//Back face
		const float backFace[] = {
			-WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f,
			-WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f,
			WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f,

			WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f,
			-WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f,
			WORLD_SCALE / 2.0f, WORLD_SCALE / 2.0f, -WORLD_SCALE / 2.0f,	
		};

		const float backFaceTexture[] = {
			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,

			0.0f, 1.0f,	
			1.0f, 1.0f,
			0.0f, 0.0f,
		};	

		addVertices(chunk, backFace, backFaceTexture, x, y, z, getBlock(x, y, z));	
	}
}

void World::buildChunk(int32_t chunkX, int32_t chunkZ)
{
	std::cerr << "Building chunk: " << chunkX << ", " << chunkZ << '\n';

	uint32_t index = ((chunkX + worldSize / (2 * CHUNK_SIZE)) * (worldSize / CHUNK_SIZE + 1) +
					 (chunkZ + worldSize / (2 * CHUNK_SIZE)));
	
	std::vector<float> chunk;

	int32_t worldChunkX = chunkX * CHUNK_SIZE,
			worldChunkZ = chunkZ * CHUNK_SIZE;

	for(int32_t x = worldChunkX; x < worldChunkX + CHUNK_SIZE; x++)
		for(int32_t y = 0; y < worldHeight; y++)
			for(int32_t z = worldChunkZ; z < worldChunkZ + CHUNK_SIZE; z++)
				addBlockVertices(chunk, x, y, z);	
	
	// 5 values per vertex
	// (x, y, z) (textureX, textureY)
	chunkVertexCount[index] = chunk.size() / 5;				

	glBindBuffer(GL_ARRAY_BUFFER, buffers[index * 2]);			
	glBufferData(GL_ARRAY_BUFFER, 
		 chunk.size() * sizeof(float), 
		 chunk.data(),
		 GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[index * 2 + 1]);	
	glBufferData(GL_ARRAY_BUFFER, 
		 chunk.size() * sizeof(float), 
		 chunk.data(),
		 GL_STATIC_DRAW);
}

void World::buildAllChunks()
{
	std::cerr << "Building all chunks...\n";

	for(int32_t x = -(int32_t)worldSize / (2 * CHUNK_SIZE); x < (int32_t)worldSize / (2 * CHUNK_SIZE); x++)
		for(int32_t z = -(int32_t)worldSize / (2 * CHUNK_SIZE); z < (int32_t)worldSize / (2 * CHUNK_SIZE); z++)
			buildChunk(x, z);
}

void World::displayWorld()
{
	for(int32_t x = -(int32_t)worldSize / (2 * CHUNK_SIZE); x < (int32_t)worldSize / (2 * CHUNK_SIZE); x++)
	{
		for(int32_t z = -(int32_t)worldSize / (2 * CHUNK_SIZE); z < (int32_t)worldSize / (2 * CHUNK_SIZE); z++)
		{
			uint32_t index = ((x + worldSize / (2 * CHUNK_SIZE)) * (worldSize / CHUNK_SIZE + 1) +
					 (z + worldSize / (2 * CHUNK_SIZE)));		
			
			glBindBuffer(GL_ARRAY_BUFFER, buffers[index * 2]);					
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
			glBindBuffer(GL_ARRAY_BUFFER, buffers[index * 2 + 1]);		
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));
			glEnableVertexAttribArray(0);			
			glEnableVertexAttribArray(1);
			glDrawArrays(GL_TRIANGLES, 0, chunkVertexCount[index]);
		}
	}
}

glm::vec3 raycast(World &world, glm::vec3 start, float yaw, float pitch, float maxdist)
{
	//TODO: better raycasting algorithm
	glm::vec3 currentPos = start;
	float step = 0.05f;

	while(glm::length(currentPos - start) < maxdist)
	{
		if(world.getBlock(
				(int32_t)floorf(currentPos.x), 
				(int32_t)floorf(currentPos.y), 
				(int32_t)floorf(currentPos.z)) != AIR)
		{
			return currentPos;
		}

		currentPos += glm::vec3(step * cosf(pitch) * sinf(yaw),
								step * sinf(-pitch),
								step * cosf(pitch) * -cosf(yaw));
	}

	return currentPos;
}
