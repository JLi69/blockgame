#ifndef __WORLD_H__
#include <stdint.h>
#include <vector>
#include <glm/glm.hpp>

enum Blocks : uint8_t
{
	AIR,
	GRASS,
	DIRT,
	STONE,
	BRICK,
	WOOD
};

const int32_t CHUNK_SIZE = 16;
const float WORLD_SCALE = 2.0f;

class World
{
	uint8_t *blocks = nullptr;
	uint32_t worldSize, worldHeight;

	unsigned int *buffers = nullptr;
	unsigned int *chunkVertexCount = nullptr;

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
	//Call this before deleting the object
	void deleteBuffers();
};

glm::vec3 raycast(World &world, glm::vec3 start, float yaw, float pitch, float maxdist);
void addVertices(std::vector<float> &chunk, 
				 const float vertices[],
				 const float textureCoords[],
				 int32_t x,
				 int32_t y,
				 int32_t z,
				 uint8_t blockType);

#endif

#define __WORLD_H__
