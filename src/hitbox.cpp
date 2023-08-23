#include "hitbox.hpp"
#include <math.h>
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

Plane::Plane(glm::vec3 point, glm::vec3 norm)
{
	normal = glm::normalize(norm);
	distance = glm::dot(normal, point);
}

Plane::Plane()
{
	normal = glm::vec3(0.0f);
	distance = 0.0f;
}

float Plane::signedDistToPlane(glm::vec3 point)
{
	return glm::dot(point, normal) - distance;
}

bool hitboxOnOrForwardPlane(Plane plane, Hitbox hitbox)
{
	glm::vec3 extents = hitbox.dimensions / 2.0f;
	float hitboxCenterDist = plane.signedDistToPlane(hitbox.position);
	float r = glm::dot(
		extents, 
		glm::vec3(fabs(plane.normal.x), fabs(plane.normal.y), fabs(plane.normal.z))
	);

	return -r <= hitboxCenterDist;
}

bool hitboxIntersectsFrustum(Frustum frustum, Hitbox hitbox)
{
	return hitboxOnOrForwardPlane(frustum.near, hitbox) &&
		   hitboxOnOrForwardPlane(frustum.far, hitbox) &&
		   hitboxOnOrForwardPlane(frustum.left, hitbox) &&
		   hitboxOnOrForwardPlane(frustum.right, hitbox) &&
		   hitboxOnOrForwardPlane(frustum.top, hitbox) &&
		   hitboxOnOrForwardPlane(frustum.bottom, hitbox);
}
