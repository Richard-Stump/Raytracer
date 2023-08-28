#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP

#include <glm/glm.hpp>

/**
 * Material for an object the scene
 */
struct Material
{
public:
	glm::dvec3	diffuse{0.0, 0.0, 0.0};
	glm::dvec3	specular{0.0, 0.0, 0.0};
	double		shininess{0.0};
};

/**
 * Represents an intersection with an object.
 */
struct Intersection
{
	Material*	material;	// The material of the object we intersected with
	glm::dvec3	pos;		// The position the intersection occured
	glm::dvec3	norm;		// The normal of the object at the intersection
	double		t;			// The T parameter along the ray for the position
};

#endif//STRUCTURES_HPP