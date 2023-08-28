#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <optional>
#include <thread>
#include <algorithm>
#include <filesystem>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Parser.hpp"
#include "Renderer.hpp"

static std::string& toLower(std::string& string)
{
    std::transform(
        string.begin(),
        string.end(),
        string.begin(),
        [](unsigned char c) { return std::tolower(c); }
    );

    return string;
}

/**
 * Prints usage info to stdout
 * 
 * @param programName The exe name that was used to call the program
 */
void printUsage(char* programName)
{
    std::cout << "Usage: " << programName << " <input file> [<options>]\n"   <<
                 "Options:\n"                                                               <<
                 "    -o <folder>   Output the rendered images in the specified folder.\n" <<
                 "    -p            Display the image while it is being rendered\n"   <<
                 "    -f <format>   The format to use for the output images:\n"                     <<
                 "              png, jpg\n" <<
                 std::endl;
}

/**
 * Parses the arguments for the program and stores them in a configuration struct.
 * 
 * @param argc The number of arguments passed to the program
 * @param argv The array of argument strings passed to the program
 * @return An optional containing the parsed configuration, or std::nullopt if
 *         an error occurred
 */
std::optional<Configuration> parseArguments(int argc, char** argv)
{
    Configuration config;

    for (int i = 0; i < argc; i++) {
        std::string arg(argv[i]);

        if (arg == "-d") {
            config.display = DisplayMode::PIXEL;
        }
        else if (arg == "-o") {
            config.outputName = argv[++i];

            if (config.outputFormat == OutputFormat::NONE) {
                config.outputFormat = OutputFormat::PNG;
            }

            char last = config.outputName[config.outputName.size() - 1];
            if (last != '/' && last != '\\') {
                config.outputName += '/';
            }
        }
        else if (arg == "-f") {
            std::string format(argv[++i]);

            format = toLower(format);

            if (format == "png") {
                config.outputFormat = OutputFormat::PNG;
            }
            else if (format == "jpg" || format == "jpeg") {
                config.outputFormat = OutputFormat::JPEG;
            }
        }
    }

    return config;
}

/**
 * Entrypoint for the program.
 * 
 * @param argc The number of arguments passed to this program
 * @param argv The array of argument strings passed to this program
 * @return The status of the program at exit.
 * 
 * @note This function is explicitly declared as using the _cdecl calling
 *       convention since the rest of the program is defined to use faster
 *       calling conventions. This function needs to be _cdecl so that
 *       windows can properly call it.
 */
int _cdecl main (int argc, char** argv)
{   
    // We need to know the file name to render. If no file is specified, show the
    // user the usage info
    if (argc < 2) {
        printUsage(argv[0]);
        return 0;
    }
    
    // Open our scene file
    std::ifstream inputFile(argv[1]);
    if (!inputFile) {
        std::cerr << "Could not open input file \"" << argv[1] << 
                     "\". Cannot continue" << std::endl;

        return -1;
    }

    // Parse the rest of the configuartion options
    std::optional<Configuration> configOpt = parseArguments(argc - 2, &argv[2]);
    if (!configOpt.has_value()) {
        return -1;
    }
    Configuration config = configOpt.value();

    // Initialize the libraries we need to use 
    if (SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Could not initialize SDL2! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0) {
        std::cerr << "Could not initialize SDL2_image: SDL_Error: " << IMG_GetError() << std::endl;
        return -1;
    }

    if (config.outputFormat != OutputFormat::NONE) {
        std::filesystem::create_directory("./" + config.outputName);
    }

    SDL_Window* window = nullptr;
    SDL_Surface* surface = nullptr;

    Animation anim;
    Parser parser(inputFile);
    parser.doParse(anim);

    std::cout << SDL_GetError() << std::endl;

    // If the user specified that they want to see the rendering, open a window with SDL
    if (config.display != DisplayMode::NONE) {
        window = SDL_CreateWindow("Raytracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, anim.width, anim.height, 0);
        surface = SDL_GetWindowSurface(window);
    }
    else {
        surface = SDL_CreateRGBSurface(0, anim.width, anim.height, 32, 0x000000FF, 0x00000FF00, 0x00FF0000, 0xFF000000);
    }
    std::cout << SDL_GetError() << std::endl;

    // Render all the frames in our scene
    renderFrames(window, surface, anim, config);
    
    // Once we are are done rendering, continue to show the last frame. Oversight: The window may not be
    // open, but SDL doesn't do anything if that is the case. Should have been accounted for though.
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
        }

        SDL_BlitSurface(surface, nullptr, SDL_GetWindowSurface(window), nullptr); 
        SDL_UpdateWindowSurface(window);

        SDL_Delay(100);
    }


    SDL_Quit();

    return 0;
}