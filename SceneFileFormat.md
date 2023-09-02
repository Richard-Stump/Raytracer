# Raytracer Scene File Format
Each scene file for the project can contain a render settings block,
and a number of keyframe blocks

## Render Settings Block
The render settings block is used to specify the settings used
for rendering the scene:
```
RenderSettings {
	Resolution		1080 720
	MaxDepth		20
	Samples			16
	Fps				60
	Loop
}
```

The re

## Keyframe Block:

### Object Definitions:
This raytracer can render spheres and planes with lights in the scene.
Each object defintion starts with the type of the object:
- Sphere
- Plane
- Light
- Camera
and then a quoted name that is used to identify that object in
later frames.