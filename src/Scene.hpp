#ifndef FRAME_HPP
#define FRAME_HPP

#include <vector>
#include <memory>
#include <string> 

#include "Objects.hpp"
#include "Structures.hpp"

/**  
 * Stores the information for a single frame, such as the objects, lights, camera
 * and the offset from the previous frame
 */
struct Frame
{
	/// List of objects in the frame
	std::vector<std::shared_ptr<Object>>	objects;

	/// List of lights in the frame
	std::vector<std::shared_ptr<Light>>		lights;
	
	/// List of names given to objects in the frame
	std::vector<std::string>				objectNames;
	
	/// List of names given to the lights in the frame
	std::vector<std::string>				lightNames;

	/// The background color for this frame 
	glm::dvec3								background;

	/// The camera for this frame
	Camera									camera;

	/// The name of the camera in this frame 
	std::string								cameraName;

	/// Time offset in seconds that this frame occurs, after the first frame
	double									timeOffset;
};

/** 
 * Stores the information needed to render an animation, including the
 * quality settings and a list of keyframes.
 */
struct Animation
{
	/// A list of keyframes to render for the animation
	std::vector<Frame>	keyFrames;

	/// Animation framerate
	int					fps			= 24;
	
	/// Width of the render
	int					width		= 720;

	/// Height of the render
	int					height		= 480;

	/// Max number of reflections to calculate
	int					maxDepth	= 4;

	/// The square root of the number of samples to render.
	int					samples		= 3;

	/// Loop the animation back to the beginning after the last frame?
	bool				loop		= false;
};

#endif//FRAME_HPP