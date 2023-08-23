#ifndef __HITBOX_H__
#include <glm/glm.hpp>

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

struct Plane
{
	float distance; //Distance of origin (0, 0, 0) to closest point on the plane
	glm::vec3 normal; //Vector that is normal to the plane (has length 1)
	//point = arbitrary point on plane
	//norm = plane's normal vector
	Plane(glm::vec3 point, glm::vec3 norm);
	Plane();
	float signedDistToPlane(glm::vec3 point);
};

struct Frustum
{
	Plane near,
		  far,
		  left,
		  right,
		  top,
		  bottom;
};

bool hitboxOnOrForwardPlane(Plane plane, Hitbox hitbox);
bool hitboxIntersectsFrustum(Frustum frustum, Hitbox hitbox);

#endif

#define __HITBOX_H__
