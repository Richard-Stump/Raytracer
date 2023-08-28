#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL2/SDL.h>
#include <string>

#include "Scene.hpp"

/**
 * The output format to use for writing the frames
 */
enum class OutputFormat
{
    /// Do not write any frames to an output folder
    NONE,

    /// Write the frames as a png
    PNG,
    
    /// Write the frames as a JPEG
    JPEG,
};

/**
 * The display mode selection for rendering to a window
 */
enum class DisplayMode
{
    /// Do not display to a window
    NONE,

    /// Display one frame at a time
    FRAME,

    /// Display after each pixel update
    PIXEL
};

/**
 * Configuration settings for the program
 */
struct Configuration
{
    /// The display mode for rendering to a window
    DisplayMode     display = DisplayMode::NONE;
    
    /// The folder to output rendered frames to
    std::string     outputName = "output/";

    /// The format to use for outputting
    OutputFormat    outputFormat = OutputFormat::NONE;
};

/**
 * Renders all the frames within the passed animation
 *
 * @param window The window to use for display, or NULL if we are not rendering to a window
 * @param surface The surface to render to
 * @param animation The animation to render
 * @param config The configuration settings for the renderer
 */
void renderFrames(SDL_Window* window, SDL_Surface* surface, Animation animation, Configuration config);

#endif//RENDERER_HPP

