#include "hitbox.hpp"

#include <iostream>

Hitbox::Hitbox(glm::vec3 pos, glm::vec3 dim)
{
	position = pos;
	dimensions = dim;
}

Hitbox::Hitbox()
{
	position = glm::vec3(0.0f);
	dimensions = glm::vec3(0.0f);
}

bool intersecting(Hitbox h1, Hitbox h2)
{
	//Adding/subtracting 0.001 is just a quick hack to hopefully account
	//for most floating point errors
	return (h1.position.x + h1.dimensions.x / 2.0f) - 0.001f > (h2.position.x - h2.dimensions.x / 2.0f) &&
		   (h1.position.x - h1.dimensions.x / 2.0f) + 0.001f < (h2.position.x + h2.dimensions.x / 2.0f) &&
		   (h1.position.y + h1.dimensions.y / 2.0f) - 0.001f > (h2.position.y - h2.dimensions.y / 2.0f) &&
		   (h1.position.y - h1.dimensions.y / 2.0f) + 0.001f < (h2.position.y + h2.dimensions.y / 2.0f) &&
		   (h1.position.z + h1.dimensions.z / 2.0f) - 0.001f > (h2.position.z - h2.dimensions.z / 2.0f) &&
		   (h1.position.z - h1.dimensions.z / 2.0f) + 0.001f < (h2.position.z + h2.dimensions.z / 2.0f); 
}

Hitbox uncollideX(Hitbox h1, Hitbox h2)
{	
	if(intersecting(h1, h2))
	{
		if(h1.position.x <= h2.position.x - h2.dimensions.x / 2.0f) 
		{
			return Hitbox(
				glm::vec3(
					h2.position.x - h1.dimensions.x / 2.0f - h2.dimensions.x / 2.0f,
					h1.position.y,
					h1.position.z
				), 
				h1.dimensions
			);
		}
		else if(h1.position.x >= h2.position.x + h2.dimensions.x / 2.0f) 
		{
			return Hitbox(
				glm::vec3(
					h2.position.x + h1.dimensions.x / 2.0f + h2.dimensions.x / 2.0f,
					h1.position.y,
					h1.position.z
				), 
				h1.dimensions
			);
		}
	}

	return h1;
}

Hitbox uncollideY(Hitbox h1, Hitbox h2)
{
	if(intersecting(h1, h2))
	{
		if(h1.position.y <= h2.position.y - h2.dimensions.y / 2.0f) 
		{
			return Hitbox(
				glm::vec3(
					h1.position.x,
					h2.position.y - h1.dimensions.y / 2.0f - h2.dimensions.y / 2.0f,
					h1.position.z
				), 
				h1.dimensions
			);
		}
		else if(h1.position.y >= h2.position.y + h2.dimensions.y / 2.0f)
		{
			return Hitbox(
				glm::vec3(
					h1.position.x,
					h2.position.y + h1.dimensions.y / 2.0f + h2.dimensions.y / 2.0f,
					h1.position.z
				), 
				h1.dimensions
			);
		}
	}

	return h1;
}

Hitbox uncollideZ(Hitbox h1, Hitbox h2)
{
	if(intersecting(h1, h2))
	{
		if(h1.position.z <= h2.position.z - h2.dimensions.z / 2.0f)
		{
			return Hitbox(
				glm::vec3(
					h1.position.x,
					h1.position.y,
					h2.position.z - h1.dimensions.z / 2.0f - h2.dimensions.z / 2.0f
				), 
				h1.dimensions
			);
		}
		else if(h1.position.z >= h2.position.z + h2.dimensions.z / 2.0f)
		{
			return Hitbox(
				glm::vec3(
					h1.position.x,
					h1.position.y,
					h2.position.z + h1.dimensions.z / 2.0f + h2.dimensions.z / 2.0f
				), 
				h1.dimensions
			);
		}
	}

	return h1;
}

Hitbox searchForBlockCollision(Hitbox h, World &world)
{
	int32_t hitboxIntX = (int32_t)floorf(h.position.x),
			hitboxIntY = (int32_t)floorf(h.position.y),
			hitboxIntZ = (int32_t)floorf(h.position.z);

	int32_t hitBoxDimX = (int32_t)ceilf(h.dimensions.x) + 2,
			hitBoxDimY = (int32_t)ceilf(h.dimensions.y) + 2,
			hitBoxDimZ = (int32_t)ceilf(h.dimensions.z) + 2;

	Hitbox block = Hitbox();

	for(int32_t x = hitboxIntX - hitBoxDimX / 2; x <= hitboxIntX + hitBoxDimX / 2; x++)
	{
		for(int32_t y = hitboxIntY - hitBoxDimY / 2; y <= hitboxIntY + hitBoxDimY / 2; y++)
		{
			for(int32_t z = hitboxIntZ - hitBoxDimZ / 2; z <= hitboxIntZ + hitBoxDimZ / 2; z++)
			{
				block = Hitbox(
					glm::vec3(x, y + 0.5f, z),
					glm::vec3(1.0f, 1.0f, 1.0f)
				);

				if(world.getBlock(x, y, z) != AIR && intersecting(h, block))
					return block;
			}		
		}
	}

	return Hitbox(); 
}
