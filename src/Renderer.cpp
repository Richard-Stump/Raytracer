#include "Renderer.hpp"

#include <iostream>

#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

/**
 * Helper template function for linearly interpolating between values
 *
 * @param a		The starting value
 * @param b		The ending value
 * @param alpha A time value (typically between 0 and 1) for interpolation
 * @return		The interpolated value
 */
template<typename T>
T lerp(T a, T b, double alpha)
{
	return (1 - alpha) * a + alpha * b;
}

/**
 * Updates the window to display the current render
 * 
 * @param window The window to render to
 */
void updateWindow(SDL_Window* window)
{
	// If there is no window, just carry on
	if (window == nullptr) return;
	
	// Keep track of the accumulated time and time of previous render
	static uint32_t lastTime = SDL_GetTicks();
	static uint32_t accTime = 0;


	uint32_t curTime = SDL_GetTicks();
	accTime += curTime - lastTime;
	lastTime = curTime;

	// Update the window every 50 milleseconds at max
	if (accTime > 50) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				exit(0);
		}

		SDL_UpdateWindowSurface(window);

		accTime = 0;
	}
}

/**
 * Computes the closest intersection with the passed ray and the passed frame
 * 
 * @param origin	The origin of the ray
 * @param dir		The direction of the ray
 * @param frame		The frame currently being rendered
 * @return			The closest intersection, or std::nullopt if there is no intersection
 */
std::optional<Intersection> closestIntersection(glm::dvec3 origin, glm::dvec3 dir, Frame& frame)
{
	bool intersection = false;	// Whether or not we've seen an intersection
	glm::dvec3 res;
	Intersection closest;		// The current closest intersection
	
	// Check against all the objects in the frame
	for (std::shared_ptr<Object> o: frame.objects) {
		auto intOpt = o->intersect(origin, dir);

		if (intOpt.has_value()) {
			Intersection i = intOpt.value();
			
			// If we have not had an intersection yet, set the intersection flag.
			// If this intersection is closer, update the closest intersecition
			if (!intersection) {
				intersection = true;
				closest = i;
			}
			else if (closest.t > i.t) {
				closest = i;
			}
		}
	}

	// return the intersection, if there is one. 
	if (intersection) {
		return std::optional<Intersection>(closest);
	}
	else {
		return std::optional<Intersection>();
	}
}

/**
 * Calculate the first intersection (not closest) with the objects in the scene. 
 * This is used for lighting calculations.
 * 
 * @param origin	The origin of the ray.
 * @param dir		The direction of the ray
 * @param frame		The frame we are rendering
 * @return			The intersection calculated
 */
std::optional<Intersection> intersection(glm::dvec3 origin, glm::dvec3 dir, Frame& frame)
{
	for (std::shared_ptr<Object> o : frame.objects) {
		auto opt = o->intersect(origin, dir);

		if (opt.has_value()) {
			return opt;
		}
	}

	return std::optional<Intersection>();
}

/**
 * Calculates phong lighting for the intersection and scene
 *
 * @param eye		The viewing location for the illumination calculation
 * @param inter		The intersection info
 * @param frame		The frame we are rendering
 * @return			The RGB value of the lighting
 * 
 * @note This is no longer used, but was left here just in case I wanted to add an 
 *       option to use it
 */
glm::dvec3 phong(glm::dvec3 eye, Intersection inter, Frame& frame)
{
	glm::dvec3 view = glm::normalize(eye - inter.pos);

	glm::dvec3 finalColor(0.0);

	// Go through all the lights in the scene and calculate all the all lighting,
	// and average them together
	for (std::shared_ptr<Light> l : frame.lights) {
		glm::dvec3 lDir = glm::normalize(l->position - inter.pos);

		auto opt = intersection(inter.pos, lDir, frame);
		if (opt.has_value())
			continue;

		double 		sDiff = glm::max(glm::dot(inter.norm, lDir), 0.0);
		glm::dvec3	diff = sDiff * inter.material->diffuse * l->diffuse;

		glm::dvec3 ref = glm::reflect(-lDir, inter.norm);

		double sSpec = glm::pow(glm::max(glm::dot(view, ref), 0.0), inter.material->shininess);
		glm::dvec3 spec = sSpec * inter.material->specular * l->specular;

		finalColor += diff + spec;
	}

	return finalColor;
}

/**
 * Calculates blinn-phong lighting for the intersection and scene
 * 
 * @param eye		The viewing location for the illumination calculation
 * @param inter		The intersection info
 * @param frame		The frame we are rendering
 * @return			The RGB value of the lighting
 */
glm::dvec3 blinn(glm::dvec3 eye, Intersection inter, Frame& frame)
{
	glm::dvec3 view = glm::normalize(eye - inter.pos);

	glm::dvec3 finalColor(0.0);

	for (std::shared_ptr<Light> l : frame.lights) {
		glm::dvec3 lDir = glm::normalize(l->position - inter.pos);

		auto opt = intersection(inter.pos, lDir, frame);
		if (opt.has_value())
			continue;

		double 		sDiff = glm::max(glm::dot(inter.norm, lDir), 0.0);
		glm::dvec3	diff = sDiff * inter.material->diffuse * l->diffuse;

		glm::dvec3 halfway = glm::normalize(view + lDir);
		double sSpec = glm::pow(glm::max(glm::dot(halfway, inter.norm), 0.0), 4.0 * inter.material->shininess);
		glm::dvec3 spec = sSpec * inter.material->specular * l->specular;

		finalColor += diff + spec;
	}

	return finalColor;
}

/**
 * Traces a ray through the specified frame of a scene
 * 
 * @param orig		The origin of the ray
 * @param dir		The direction of the ray
 * @param frame		The frame to render
 * @param maxDepth	The maximum number of reflectoins
 * @return			The traced color
 * 
 * @note While cleaning up the code a bit to upload to github, I found a few potential problems:
 * - Lighting for reflections is always calculated using the camera position. It should probaby
 *   be based off the ray we are tracing, not the camera's position
 * - If the maximum depth is reached, we return black, not the background color
 * - The calculation of the reflection color with the objects color probably needs another look.
 *   Maybe the diffuse and specular components should be seperate? I'm not sure about this one.
 * 
 * I am currently too busy with helping my Dad in the garage, applying to places, and a minecraft
 * project to fix these at the moment. 
 */
glm::dvec3 trace(glm::dvec3 orig, glm::dvec3 dir, Frame& frame, int maxDepth)
{	
	// If we reached our max-depth, return 0
	if (maxDepth == 0)
		return { 0.0, 0.0, 0.0 };

	// Get the closest intersection, if any
	auto interOpt = closestIntersection(orig, dir, frame);

	// Return the background if there is no intersection
	if (!interOpt.has_value())
		return frame.background;
	
	Intersection inter = interOpt.value();

	glm::dvec3 ref = glm::reflect(dir, inter.norm);

	// Trace the reflection and get it's color
	glm::dvec3 refColor = trace(inter.pos, ref, frame, maxDepth - 1);
		
	return refColor * inter.material->specular + blinn(frame.camera.position, interOpt.value(), frame);
}

void renderFrame(SDL_Window* window, SDL_Surface* surface, Frame& frame, int maxDepth, int samples, Configuration config)
{
	//Precalculate values that will be used for each pixel in the scene
	Camera& camera = frame.camera;
	
	glm::dvec3	l	= glm::normalize(camera.lookat - camera.position);
	glm::dvec3	v	= glm::normalize(glm::cross(l, camera.up));
	glm::dvec3	u	= glm::cross(v, l);
	glm::dvec3	eye = camera.position;

	double a = (double)surface->w / (double)surface->h;
	double d = 1.0 / glm::tan(camera.fov / 2.0);

	glm::dvec3 ll = eye + d * l - a * v - u;

	glm::dvec3 cx = 2.0* a * v / (double)surface->w;
	glm::dvec3 cy = 2.0 * u / (double)surface->h;

	// Use OpenMP to render many pixels at once. Parallelizing multiple rows 
	// rather than individual pixels proved to be quicker when displaying to a window
	#pragma omp parallel for
	for (int py = 0; py < surface->h; py++) {
		for (int px = 0; px < surface->w; px++) {
			glm::dvec3 color(0.0);
			
			// Calculate subpixels (if enabled) 
			for (int sy = 0; sy < samples; sy++) {
				for (int sx = 0; sx < samples; sx++) {
					double x = (double)px + (double)sx / (double)samples;
					double y = (double)py + (double)sy / (double)samples;

					//calculate the ray for this pixel
					glm::dvec3 p = ll + cx * x + cy * y;
					glm::dvec3 dir = glm::normalize(p - eye);

					color += trace(eye, glm::normalize(dir), frame, 64);
				}
			}

			// Average the colors of our subpixels
			color /= (double)(samples * samples);
			
			uint8_t r = glm::floor(color.r >= 1.0 ? 255 : color.r * 256.0);
			uint8_t g = glm::floor(color.g >= 1.0 ? 255 : color.g * 256.0);
			uint8_t b = glm::floor(color.b >= 1.0 ? 255 : color.b * 256.0);

			((uint32_t*)surface->pixels)[(surface->h - py - 1) * surface->w + px] = (uint32_t)SDL_MapRGB(surface->format, r, g, b);
			
			// Update the window if we have one
			if(window)
				updateWindow(window);
		}
	}
}

/**
 * Interpolates the two passed frames based on the time value
 * 
 * @param f1	The start frame
 * @param f2	The end frame
 * @param alpha	The time value
 * @return		The interpolated frame
 */
Frame interpolateFrames(Frame& f1, Frame& f2, double alpha)
{
	// The new frame we calculated
	Frame newFrame{};
	
	//interpolate objects
	int i = 0;
	for ( ; i < f1.objects.size(); i++) {
		std::shared_ptr<Object> newObject;

		// Interpolate the object based on it's type
		if (typeid(*f1.objects[i]) == typeid(Sphere)) {
			newObject = std::make_shared<Sphere>();
			newObject->interpolate(*f1.objects[i], *f2.objects[i], alpha);
			newFrame.objects.push_back(newObject);
		}
		else if (typeid(*f1.objects[i]) == typeid(Plane)) {
			newObject = std::make_shared<Plane>();
			newObject->interpolate(*f1.objects[i], *f2.objects[i], alpha);
			newFrame.objects.push_back(newObject);
		}
	}

	// Interpolate the lights in the scene
	for (i = 0; i < f1.lights.size(); i++) {
		std::shared_ptr<Light> newLight = std::make_shared<Light>();
		newLight->interpolate(*f1.lights[i], *f2.lights[i], alpha);
		newFrame.lights.push_back(newLight);
	}

	// Interpolate the background and camera 
	newFrame.background = lerp(f1.background, f2.background, alpha);
	newFrame.camera.interpolate(f1.camera, f2.camera, alpha);

	return newFrame;
}

void renderFrames(SDL_Window* window, SDL_Surface* surface, Animation animation, Configuration config) 
{
	// NOTE: The logic of this function could probably be simplified to have a single loop and less
	//       branching based on special conditions

	int frameNumber = 0;

	if (animation.keyFrames.size() < 0) {
		// Can't render if there are no keyframes
		std::cerr << "Error: No Keyframes Found" << std::endl;
		return;
	}
	else if (animation.keyFrames.size() == 1) {
		// If we only have one frame, render it without looping and without interpolation
		renderFrame(window, surface, animation.keyFrames[0], animation.maxDepth, animation.samples, config);
		if (config.outputFormat != OutputFormat::NONE) {
			std::string path = config.outputName + "frame_0";

			if (config.outputFormat == OutputFormat::PNG) {
				path += ".png";

				IMG_SavePNG(surface, path.c_str());
			}
			if (config.outputFormat == OutputFormat::JPEG) {
				path += ".jpg";
				IMG_SaveJPG(surface, path.c_str(), 70);
			}
		}
		return;
	}

	// for each pair of frames
	int numKeyFrames = animation.keyFrames.size();

	for (int i = 0; i < numKeyFrames - (animation.loop ? 0 : 1); i++) {
		// Get the first and last frames in the animation
		Frame& startFrame = animation.keyFrames[i];
		Frame& endFrame = animation.keyFrames[(i + 1) % numKeyFrames];

		// Calculate our timestep between keyframes
		int fps = animation.fps;
		double deltaTime = startFrame.timeOffset;
		int frameCount = deltaTime * fps;
		double timeStep = deltaTime / (double)frameCount;
		
		double alpha = 0.0;

		for (int j = 0; j < frameCount; j++, alpha += timeStep) {
			std::cout << "Rendering frame " << frameNumber << ": " << std::flush;
			
			uint32_t renderStartTime = SDL_GetTicks();

			// If we have the start or end frame, render the frame as-is without interpolation. Otherwise,
			// interpolate the nearest two frames
			if (j == 0) {
				renderFrame(window, surface, startFrame, animation.maxDepth, animation.samples, config);
			}
			else if (j == frameCount - 1) {
				renderFrame(window, surface, endFrame, animation.maxDepth, animation.samples, config);
			}
			else {
				Frame interpFrame = interpolateFrames(startFrame, endFrame, alpha);
				renderFrame(window, surface, interpFrame, animation.maxDepth, animation.samples, config);
			}

			uint32_t renderEndTime = SDL_GetTicks();
			double seconds = (double)(renderEndTime - renderStartTime) / 1000.0;

			std::cout << "  Took " << seconds << "s to render" << std::endl;

			// Output the frame we just rendered
			if (config.outputFormat != OutputFormat::NONE) {
				std::string path = config.outputName + "frame_" + std::to_string(frameNumber);

				if (config.outputFormat == OutputFormat::PNG) {
					path += ".png";

					IMG_SavePNG(surface, path.c_str());
				}
				if (config.outputFormat == OutputFormat::JPEG) {
					path += ".jpg";
					IMG_SaveJPG(surface, path.c_str(), 70);
				}
			}

			frameNumber++;
		}
	}
}
