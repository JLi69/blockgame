#ifndef __WORLD_H__
#include <stdint.h>
#include <vector>

enum Blocks : uint8_t
{
	AIR,
	GRASS,
	STONE
};

class World
{
	uint8_t *blocks;
	uint32_t worldSize, worldHeight;

	unsigned int *buffers;
	std::vector<float> *chunks;

	void addBlockVertices(int32_t x, 
						  int32_t y,
						  int32_t z,
						  int32_t chunkX,
						  int32_t chunkZ);
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

#endif

#define __WORLD_H__
