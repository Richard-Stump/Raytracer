#ifndef OBJECTS_HPP
#define OBJECTS_HPP

#include <optional>
#include <string>
#include <glm/glm.hpp>

#include "Structures.hpp"

// Forward declaration for the tokenizer
class Tokenizer;

/**
 * Abstract class representing an object in the scene
 */
class Object
{
public:	
	/**
	 * Check where, if any, intersection between the object and a ray occurs
	 * 
	 * \param origin		The origin of the ray
	 * \param direction		The direction of the ray
	 * \return				The intersection info, or std::nullopt if no intersection exists
	 */
	virtual std::optional<Intersection> intersect(glm::dvec3 origin, glm::dvec3 direction) {
		return std::optional<Intersection>();
	}
	
	/**
	 * Parses a property for the given object. This allows each object to have its own
	 * set of properties in the scene file
	 * 
	 * \param name		The name of the property
	 * \param tokenizer	The tokenizer for the scene file
	 */
	virtual void parseProperty(std::string& name, Tokenizer& tokenizer);

	/// The string identifier given to this object. Used for the interpolation code
	std::string	name;

	/**
	 * Sets this object's properties as the linear interpolation between the two passed objects.
	 * This allows for keyframing within the scene
	 * 
	 * \param a		The starting object state
	 * \param b		The ending object state
	 * \param alpha	The time value, between 0 and 1, for the interpolation
	 */
	virtual void interpolate(Object& a, Object& b, double alpha);

	// The material used for rendering this object
	Material	material;
};

/**
 * A sphere that can appear in the scene
 */
class Sphere : public Object
{
public:

	/**
	 * Check where, if any, intersection between the sphere and a ray occurs
	 *
	 * \param origin		The origin of the ray
	 * \param direction		The direction of the ray
	 * \return				The intersection info, or std::nullopt if no intersection exists
	 */
	std::optional<Intersection> intersect(glm::dvec3 origin, glm::dvec3 direction);

	/**
	 * Parses a property for the sphere. This allows each object to have its own
	 * properties in the scene file
	 *
	 * \param name		The name of the property
	 * \param tokenizer	The tokenizer for the scene file
	 */
	void parseProperty(std::string& name, Tokenizer& tokenizer);
	
	/**
	 * Sets this object's properties as the linear interpolation between the two passed objects.
	 * This allows for keyframing within the scene
	 *
	 * \param a		The starting object state
	 * \param b		The ending object state
	 * \param alpha	The time value, between 0 and 1, for the interpolation
	 */
	void interpolate(Object& a, Object& b, double alpha);

	/// The position of the center of the sphere in space
	glm::dvec3	position{0.0, 0.0, 0.0};

	/// The radius of the sphere
	double		radius{1.0};
};

/**
 * A plane that can appear in the scene
 */
class Plane : public Object
{
public:
	/**
	 * Check where, if any, intersection between the plane and a ray occurs
	 *
	 * \param origin		The origin of the ray
	 * \param direction		The direction of the ray
	 * \return				The intersection info, or std::nullopt if no intersection exists
	 */
	std::optional<Intersection> intersect(glm::dvec3 origin, glm::dvec3 direction);

	/**
	 * Parses a property for the plane. This allows each object to have its own
	 * set of properties in the scene file
	 *
	 * \param name		The name of the property
	 * \param tokenizer	The tokenizer for the scene file
	 */
	void parseProperty(std::string& name, Tokenizer& tokenizer);

	/**
	 * Sets this object's properties as the linear interpolation between the two passed objects.
	 * This allows for keyframing within the scene
	 *
	 * \param a		The starting object state
	 * \param b		The ending object state
	 * \param alpha	The time value, between 0 and 1, for the interpolation
	 */
	void interpolate(Object& a, Object& b, double alpha);

	/// A point on the plane
	glm::dvec3	point{0.0, 0.0, 0.0};

	/// The normal vector to the plane
	glm::dvec3	norm{0.0, 1.0, 0.0};
};

/**
 * A triangle that can appear in the scene
 */
class Triangle : public Object
{
public:
	/**
	 * Check where, if any, intersection between the triangle and a ray occurs
	 *
	 * \param origin		The origin of the ray
	 * \param direction		The direction of the ray
	 * \return				The intersection info, or std::nullopt if no intersection exists
	 */
	std::optional<Intersection> intersect(glm::dvec3 origin, glm::dvec3 direction);

	/**
	 * Parses a property for the triangle. This allows each object to have its own
	 * set of properties in the scene file
	 *
	 * \param name		The name of the property
	 * \param tokenizer	The tokenizer for the scene file
	 */
	void parseProperty(std::string& name, Tokenizer& tokenizer);

	/**
	 * Sets this object's properties as the linear interpolation between the two passed objects.
	 * This allows for keyframing within the scene
	 *
	 * \param a		The starting object state
	 * \param b		The ending object state
	 * \param alpha	The time value, between 0 and 1, for the interpolation
	 */
	void interpolate(Triangle& a, Triangle& b, double alpha);

	/// The three vertices for the triangle
	glm::dvec3	v1{ -1, -1, 0 }, v2{1, -1, 0}, v3{0, 1, 0};
	
	/// The precomputed normal for the triangle
	glm::dvec3	norm{0, 0, -1};
};

/**
 * Represents the camera in the scene
 */
class Camera
{
public:
	/// The current position of the camera
	glm::dvec3	position{0.0, 0.0, 0.0};	
	
	/// The point that the camera is looking at
	glm::dvec3	lookat{0.0, 1.0, 0.0};
	
	/// The up vector used to calculate the lookat matrix
	glm::dvec3	up{0.0, 1.0, 0.0};

	/// The vertical field of view for the camera
	double		fov{90.0};

	/**
	 * Sets this object's properties as the linear interpolation between the two passed objects.
	 * This allows for keyframing within the scene
	 *
	 * \param a		The starting object state
	 * \param b		The ending object state
	 * \param alpha	The time value, between 0 and 1, for the interpolation
	 */
	void interpolate(Camera& a, Camera& b, double alpha);

	/**
	 * Parses a property for the camera. This allows each object to have its own
	 * set of properties in the scene file
	 *
	 * \param name		The name of the property
	 * \param tokenizer	The tokenizer for the scene file
	 */
	void parseProperty(std::string& name, Tokenizer& tokenizer);
};

/**
 * Represents a light within the scene
 */
class Light
{
public:
	/// The position of the light
	glm::dvec3	position;

	/// The light's diffuse color
	glm::dvec3	diffuse;

	/// The light's specular color
	glm::dvec3	specular;

	/**
	 * Sets this object's properties as the linear interpolation between the two passed objects.
	 * This allows for keyframing within the scene.
	 *
	 * \param a		The starting object state
	 * \param b		The ending object state
	 * \param alpha	The time value, between 0 and 1, for the interpolation
	 */
	void interpolate(Light& a, Light& b, double alpha);

	/**
	 * Parses a property for the light. This allows each object to have its own
	 * set of properties in the scene file.
	 *
	 * \param name		The name of the property
	 * \param tokenizer	The tokenizer for the scene file
	 */
	void parseProperty(std::string& name, Tokenizer& tokenizer);
};

#endif//OBJECTS_HPP
