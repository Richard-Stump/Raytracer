#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP

#include <glm/glm.hpp>

/**
 * Material for an object the scene
 */
struct Material
{
public:
	/// The diffuse color for this material
	glm::dvec3	diffuse{0.0, 0.0, 0.0};

	/// The specular color for this material
	glm::dvec3	specular{0.0, 0.0, 0.0};

	/// The shininess for this material
	double		shininess{0.0};
};

/**
 * Represents an intersection with an object.
 * 
 * This is all of the information needed from a specific intersection
 */
struct Intersection
{
	/// The material found at the intersection point
	Material*	material;

	/// The point that the intersection occured.
	glm::dvec3	pos;

	/// The normal to the surface at the point of intersection 
	glm::dvec3	norm;

	/// The T parameter along the ray that the intersection occured
	double		t;	
};

#endif//STRUCTURES_HPP