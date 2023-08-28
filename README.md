# Raytracer
A simple raytracer project I created in Spring 2022 for CS 434. 

![An animation created with the raytracer](./images/raytracer.gif)

This program reads in a scene from text file, and then renders the
frames interpolated from the keyframes in that text file. It uses OpenMP to speed up
rendering of the frames, and calculates everything on the CPU.

## Running
The program must be run from a command line, with the input of the following form:
```
lab02.exe <scene file> [options]
```
where `<scene file>` is a text file specifying the sceen to render. The available
options are as follows:

| Option | Description |
|--------|-------------|
| `-o <folder>` | Ouptut the frames as individual images to the specified folder |
| `-d` | Display the frames to a window as they are being rendered |
| `-f <format>` | The output format for the frames. Valid values for `<format>` are `png` and `jpg` |

## Input files
This program reads in a scene from a text file. Each text file contains a 
list of renderer settings and a list of keyframes. Each keyframe can contain
a number of objects. When rendering, the various objects can be interpolated
between the keyframes to produce a simple animation. More info on the file
format can be found [here](./SceneFileFormat.md)

## Building
The project can be built right in Visual Studio by opening up the project, and clicking build.

## Misc. Notes

Given the time constraints I had at the time, there are some oversights with the code I 
would love to go back and fix given the time:
- The parsing code could use a huge rework. When I originally wrote it, I had not taken
  a compilers course, and did not know how to write a good parser.
- Parts of the code lack sanity checks that I did not notice the need for at the time.
  Things like unchecked casts, which never occur if your input file is written correctly.
- Parts of the rendering code are flawed, such as reflections being off.
- IIRC, having multiple planes in a scene can cause issues.
- Probably lots of optimizing that could be done to this code to make it more efficient

I may come back and fix these if I get the time, or I may rewrite the program using what I
have learned since my initial attempt at this project over a year ago.
