# Perspective-Renderer
This is my 3D OpenGL Renderer that I'm using to experiment with different OpenGL techniques as well as do my graphics assignments.

Unlike my previous frameworks, this renderer does not attempt to fully abstract away OpenGL calls, and I feel as if this is for the better since OpenGL calls are so inherently
tied to the techniques and optimizations rendering needs.

# Dependancies
All dependancies come with the build. They are
* Glad
* CMake
* GLFW
* ImGui
* GLM
* stb_image
* tinyobjloader

# Current Features
Currently, the renderer features an easy way of adding new passes for rendering. It also features

* An auto-reloading shader system
* ImGui support for shader errors
* ImGui support for consoles
* A seperate thread for loading obj models
* 3D Camera and environment setup
* glsl #include<> is supported
* Deferred Rendering Pipeline for lighting objects
* Forward Rendering Pipeline for debug objects

## Some pictures

# To Build

Use CMake to build. It should work out of the box as all the libraries are packaged together and built from source.

# To Execute

The default args attempt to load a bunch of factory models available in my own files (they're not uploaded because it would be over 1 GB).
However, passing an argument to the command will cause it to load the obj.

# Credits
* DigiPen Institute of Technology Professor Pushpak Karnick for providing the slides and maths
* [Prospect Renderer](https://github.com/Shimmen/Prospect) - For giving the inspiration and a lot of the technical know-hows to structure my code.
