#ifndef __WORLD_H__
#include <stdint.h>
#include <vector>
#include <glm/glm.hpp>

enum Blocks : uint8_t
{
	AIR,
	GRASS,
	STONE,
	BRICK
};

class World
{
	uint8_t *blocks;
	uint32_t worldSize, worldHeight;

	unsigned int *buffers;
	unsigned int *chunkVertexCount;

	void addBlockVertices(std::vector<float> &chunk,
						  int32_t x, 
						  int32_t y,
						  int32_t z);
public:
	//World ranges from
	//x: -size / 2 -> size / 2
	//y: 0 -> height
	//z: -size / 2 -> size / 2
	World(uint32_t size, uint32_t height);
	~World();

	void generateWorld();
	uint8_t getBlock(int32_t x, int32_t y, int32_t z);
	void setBlock(int32_t x, int32_t y, int32_t z, uint8_t block);
	void buildChunk(int32_t chunkX, int32_t chunkZ);
	void buildAllChunks();
	void displayWorld();	
};

glm::vec3 raycast(World &world, glm::vec3 start, float yaw, float pitch, float maxdist);

#endif

#define __WORLD_H__
