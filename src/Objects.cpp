#include "Objects.hpp"

#include <glm/glm.hpp>

#include "Parser.hpp"
#include "Structures.hpp"

/// The epsilon distance for comparing if two floating point numbers are close
/// enough to be equal
const double EPSILON = 1e-8;

/**
 * Helper template function for linearly interpolating between values
 * 
 * \param a		The starting value
 * \param b		The ending value
 * \param alpha A time value (typically between 0 and 1) for interpolation
 * \return		The interpolated value
 */
template<typename T>
T lerp(T a, T b, double alpha)
{
	return (1 - alpha) * a + alpha * b;
}

void Object::parseProperty(std::string& name, Tokenizer& tokenizer)
{
	// Since a generic object doesn't have any properties, discard the line
	tokenizer.discardLine();
}

void Object::interpolate(Object& a, Object& b, double alpha)
{
	// Interpolate the material properties for this object
	material.diffuse = lerp(a.material.diffuse, b.material.diffuse, alpha);
	material.specular = lerp(a.material.specular, b.material.specular, alpha);
	material.shininess = lerp(a.material.shininess, b.material.shininess, alpha);
}

std::optional<Intersection> Sphere::intersect(glm::dvec3 orig, glm::dvec3 dir)
{
	// The formula used for calculating the interesection with a sphere was given
	// in the class slides. 

	// Calculate the ray from the sphere's center to the origin of the ray
	glm::dvec3 omc = orig - position;
	
	double b = 2 * glm::dot(dir, omc);
	double c = glm::dot(omc, omc) - radius * radius;

	// dx(ox - cx) + dy(oy - cy)

	double disc = b * b - 4 * c;

	// If our discriminant is less than, or close to zero, we do not have an intersection
	if (disc <= EPSILON)
		return std::optional<Intersection>();

	double rt = glm::sqrt(disc);
	double t1 = (-b + rt) / 2.0;
	double t2 = (-b - rt) / 2.0;

	if (t1 > t2)
		std::swap(t1, t2);

	glm::dvec3 point;
	double t = t1;

	if (t1 < -EPSILON && t2 < EPSILON)
		return std::optional<Intersection>();
	else if (t1 < 0.0) {
		point = orig + t2 * dir;
		t = t2;
	}
	else {
		point = orig + t1 * dir;
		t = t1;
	}

	return std::optional<Intersection>({
		&material,
		point,
		glm::normalize(point - position),
		t
	});
}

void Sphere::parseProperty(std::string& name, Tokenizer& tokenizer)
{
	if (name == "position") {
		// Read in 3 doubles for the position
		position = {
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
		};
	}
	else if (name == "diffuse") {
		// Read in 3 doubles for the diffuse color
		material.diffuse = {
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
		};

	}
	else if (name == "specular") {
		// Read in 3 doubles for the specular color
		material.specular = {
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
		};
	}
	else if (name == "shininess") {
		material.shininess = tokenizer.nextDouble();
	}
	else if (name == "radius") {
		radius = tokenizer.nextDouble();
	}
	else {
		// If our property is unkown, discard the line and pretend like there is no issue
		tokenizer.discardLine();
		std::cout << "Unknown property \"" << name << "\"" << std::endl;
	}
}

void Sphere::interpolate(Object& a, Object& b, double alpha)
{
	// First, cast each object to a sphere object. Each object should be a sphere
	Sphere& ca = static_cast<Sphere&>(a);
	Sphere& cb = static_cast<Sphere&>(b);

	// Interpolate the object properties
	Object::interpolate(a, b, alpha);
	
	// Interpolate the sphere properties
	position = lerp(ca.position, cb.position, alpha);
	radius = lerp(ca.radius, cb.radius, alpha);
}

std::optional<Intersection> Plane::intersect(glm::dvec3 origin, glm::dvec3 direction)
{
	// The formula used for calculating the interesection with a plane was given
	// in the class slides. 
	double ddn = glm::dot(direction, norm);

	if (ddn > -EPSILON && ddn < EPSILON)
		return std::optional<Intersection>();

	double t = glm::dot(point - origin, norm) / ddn;

	if (t < EPSILON)
		return std::optional<Intersection>();

	glm::dvec3 p = origin + direction * t;

	return std::optional<Intersection>({
		&material,
		p,
		norm,
		t
	});
}

void Camera::parseProperty(std::string& name, Tokenizer& tokenizer)
{
	if (name == "position") {
		position = {
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
		};
	}
	else if (name == "lookat") {
		lookat = {
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
		};
	}
	else if (name == "up") {
		up = {
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
		};
	}
	else if (name == "fov") {
		fov = tokenizer.nextDouble();
	}
	else {
		tokenizer.discardLine();
		std::cout << "Unknown property \"" << name << "\"" << std::endl;
	}
}

void Camera::interpolate(Camera& a, Camera& b, double alpha)
{
	// Interpolate all the properties for the camera. We simply interpolate the 3
	// camera vectors, and recompute the lookat matrix each frame
	position = lerp(a.position, b.position, alpha);
	lookat = lerp(a.lookat, b.lookat, alpha);
	up = lerp(a.up, b.up, alpha);
	fov = lerp(a.fov, b.fov, alpha);
}

void Plane::parseProperty(std::string& name, Tokenizer& tokenizer)
{
	if (name == "point") {
		point = {
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
		};
	}
	else if (name == "normal") {
		norm = glm::normalize(glm::dvec3{
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
		});
	}
	else if (name == "diffuse") {
		material.diffuse = {
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
		};

	}
	else if (name == "specular") {
		material.specular = {
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
		};
	}
	else if (name == "shininess") {
		material.shininess = tokenizer.nextDouble();
	}
	else {
		tokenizer.discardLine();
		std::cout << "Unknown property \"" << name << "\"" << std::endl;
	}
}

std::optional<Intersection> Triangle::intersect(glm::dvec3 origin, glm::dvec3 direction)
{
	// The formula used for calculating the interesection with a triangle was given
	// in the class slides. 

	glm::dvec3 p = v2 - v1;
	glm::dvec3 q = v3 - v1;
	glm::dvec3 tmp1 = glm::cross(direction, q);
	double dot1 = glm::dot(tmp1, p);

	if (dot1 > -EPSILON && dot1 < EPSILON)
		return std::optional<Intersection>();

	double f = 1.0 / dot1;
	glm::dvec3 s = origin - v1;
	double u = f * glm::dot(s, tmp1);
	
	if (u < 0.0f || u > 1.0f)
		return std::optional<Intersection>();

	glm::dvec3 tmp2 = glm::cross(s, p);
	double v = f * glm::dot(direction, tmp2);
	if(v < 0.0f || u + v > 1.0f)
		return std::optional<Intersection>();

	double t = f * dot(q, tmp2);

	//We have an intersection
	glm::dvec3 point = (1 - u - v) * v1 + u * v2 + v * v3;
	
	// Calculate the time value along the ray that the intersection occurs.
	double rayTime = glm::dot(point - origin, direction);

	return std::optional<Intersection>({
		&material,
		point,
		norm,
		rayTime
	});
}

void Triangle::parseProperty(std::string& name, Tokenizer& tokenizer)
{
	if (name == "v1") {
		v1 = {
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
		};
		norm = glm::normalize(glm::cross(v2 - v1, v3 - v1));
	}
	else if (name == "v2") {
		v2 = {
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
		};
		norm = glm::normalize(glm::cross(v2 - v1, v3 - v1));
	}
	else if (name == "v3") {
		v3 = {
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
		};
		norm = glm::normalize(glm::cross(v2 - v1, v3 - v1));
	}
	else if (name == "diffuse") {
		material.diffuse = {
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
		};

	}
	else if (name == "specular") {
		material.specular = {
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
		};
	}
	else if (name == "shininess") {
		material.shininess = tokenizer.nextDouble();
	}
	else {
		tokenizer.discardLine();
		std::cout << "Unknown property \"" << name << "\"" << std::endl;
	}
}

void Triangle::interpolate(Triangle& a, Triangle& b, double alpha)
{
	// Interpolate the properties common to all objects
	Object::interpolate(a, b, alpha);

	// Interpolate the position of the vertices
	v1 = lerp(a.v1, b.v1, alpha);
	v2 = lerp(a.v2, b.v2, alpha);
	v3 = lerp(a.v3, b.v3, alpha);

	// Since our vertices moved, we need to recompute the normal. This seemed better
	// than interpolating the normals
	norm = glm::normalize(glm::cross(v2 - v1, v3 - v1));
}

void Plane::interpolate(Object& a, Object& b, double alpha) {

	// Interpolate properties common to all objects
	Object::interpolate(a, b, alpha);

	// Cast the objects to planes
	Plane& ca = static_cast<Plane&>(a);
	Plane& cb = static_cast<Plane&>(b);

	// Interpolate the point and normal of the planes
	point = lerp(ca.point, cb.point, alpha);
	norm = glm::normalize(lerp(ca.norm, cb.norm, alpha));
}

void Light::parseProperty(std::string& name, Tokenizer& tokenizer)
{
	if (name == "position") {
		position = {
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
		};
	}
	else if (name == "diffuse") {
		diffuse = {
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
		};
	}
	else if (name == "specular") {
		specular = {
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
			tokenizer.nextDouble(),
		};
	}
	else {
		tokenizer.discardLine();
		std::cout << "Unknown property \"" << name << "\"" << std::endl;
	}
}

void Light::interpolate(Light& a, Light& b, double alpha)
{
	// Interpolate the values for the light. 
	position = lerp(a.position , b.position , alpha);
	diffuse = lerp(a.diffuse , b.diffuse , alpha);
	specular = lerp(a.specular, b.specular, alpha);
}
