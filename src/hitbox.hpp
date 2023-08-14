#ifndef __HITBOX_H__
#include <glm/glm.hpp>
#include "world.hpp"

struct Hitbox
{
	glm::vec3 position;
	glm::vec3 dimensions;
	Hitbox(glm::vec3 pos, glm::vec3 dim);
	Hitbox();
};

bool intersecting(Hitbox h1, Hitbox h2);

Hitbox uncollideX(Hitbox h1, Hitbox h2);
Hitbox uncollideY(Hitbox h1, Hitbox h2);
Hitbox uncollideZ(Hitbox h1, Hitbox h2);

//Returns the hitbox of a block that the hitbox is colliding with
Hitbox searchForBlockCollision(Hitbox h, World &world);

#endif

#define __HITBOX_H__
