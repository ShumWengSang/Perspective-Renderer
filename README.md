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
* Oct Tree generation 
* BSP Tree generation
* JSON loading of trees
* Loading of FBX models and animations
* Playing FBX animations 
* Generating a bezier curve path based on input points
* 

My work on Inverse Kinematics as well as a physics simluator can be accessed through the Worldspace branch.



## Some pictures
A nice console so that anything I print can be seen in the gui.

![Powerplant](/images/console.png)

This is the power plant model that I use for seeing large amount of vertices and models for loading
I don't have a texture file for this, so I just created a simple purple texture for it and my Phong lighting takes care of the rest.

![Powerplant](/images/Powerplant.png)

The geometry pass stores the view-position, depth, diffuse, and normals. Here is a visualization of it, rendered in ImGui.

![Powerplant](/images/Powerplant_gbuffer.png)

There is a light pass where I take the GBuffer contents and do Phong lighting on it. This is a visualization of the 
light pass.

![Powerplant](/images/Powerplant_lightpass.png)

This is a cool system inspired by Prospect, where the shader gets auto-reloaded and we can use #include<> with it. ImGui can also
show what errors the file has.

Suppose we have three files...

ShaderTypes.h
```c++
#ifndef SHADER_TYPES_H
#define SHADER_TYPES_H

#define Color vec4

struct DirectionalLight // Included in C++ project and GLSL code!
{
	vec4 worldDirection;
    vec4 viewDirection;

    Color diffuseColor;
    Color ambientColor;
    Color specularColor;

	// x = softness, yzw = unused
	vec4 softness;
};
#endif // SHADER_TYPES_H

```

Common.glsl
```c++
#ifndef COMMON_GLSL
#define COMMON_GLSL

#include <ShaderTypes.h>

#define PI     (3.14159265358979323846)
#define TWO_PI (2.0 * PI)

// color.rgb is the color [0, 1] and color.a is [0, 1] but maps to a [0, 63] exponential scale y=2^(6x)-1
// see shader_types.h for definitions
vec3 rgbFromColor(Color color)
{
    float x = color.a;
    float y = pow(2.0, 5.0 * x) - 1.0;
    return vec3(color.rgb * y);
}

float lengthSquared(vec2 v)
{
    return dot(v, v);
}

float lengthSquared(vec3 v)
{
    return dot(v, v);
}

float square(float x)
{
    return x * x;
}

float saturate(float x)
{
    return clamp(x, 0.0, 1.0);
}
```
and our glsl code as 
PowerPlant.frag.glsl
```c++
#version 460

#include <common.glsl>

#include <ShaderLocations.h>
#include <CameraUniforms.h>

in vec2 v_tex_coord;
in vec3 v_position;
in vec3 v_normal;

in vec4 v_curr_proj_pos;

PredefinedUniformBlock(CameraUniformBlock, camera);

uniform vec3 u_mat_ambient;
uniform vec3 u_mat_diffuse;
uniform vec3 u_mat_specular;
uniform float u_mat_shininess;


PredefinedOutput(vec3, o_g_buffer_diffuse);
PredefinedOutput(vec3, o_g_buffer_ambient);
PredefinedOutput(vec4, o_g_buffer_specular);
PredefinedOutput(vec3, o_g_buffer_viewPos)
PredefinedOutput(vec3, o_g_buffer_normal);

void main()
{
    o_g_buffer_diffuse = u_mat_diffuse;
    o_g_buffer_ambient = u_mat_ambient;
    o_g_buffer_specular = vec4(u_mat_specular, u_mat_shininess);
    o_g_buffer_viewPos = v_position;
    o_g_buffer_normal = normalize(v_normal);
}
```
We can get a nice error message telling us where it went wrong, as well as the final output post all the #includes<>!

![Powerplant](/images/Powerplant_shader.png)

This here is a sphere model I've loaded in

![Powerplant](/images/sphere.png)

And we can see its faces normals for debug purposes

![Powerplant](/images/sphere_facedebug.png)

And vertex normals as well!

![Powerplant](/images/sphere_vertexdebug.png)

Here is another picture of the light pass on the sphere. We can see the ImGui on the left showing us what the lightpass output is.

![Powerplant](/images/sphere_lightpass.png)

![BSP](https://user-images.githubusercontent.com/8475959/113262967-11fd0680-9286-11eb-8d95-bb816df38503.png)

![BSPSphere](https://user-images.githubusercontent.com/8475959/113262973-13c6ca00-9286-11eb-83b0-4b0825fed472.png)

![OctTree](https://user-images.githubusercontent.com/8475959/113262976-14f7f700-9286-11eb-9b21-e2f3aadb0760.png)

![OctTreeModel](https://user-images.githubusercontent.com/8475959/113262939-0a3d6200-9286-11eb-971a-7626b44f1315.png)


https://user-images.githubusercontent.com/8475959/113263025-23461300-9286-11eb-8fd5-3f44174c0d62.mp4

Here is a FBX model loaded and animated properly
![Whale](/images/whale.gif)

Generation of bezier curve paths using ImPlot
![Zoom](/images/zoom2.gif)

Mixing inverse kinematics as well as moving animation to make monster's head reach out to destination
![ik](/images/ik4.gif)

Implementation of rigidbody physics: Pendulum
![pendulum](/images/pendulum.gif)

Implementation of rigidbody physics: Double Pendulum

https://github.com/ShumWengSang/Perspective-Renderer/tree/main/images/dualpendulum.mp4

Implementation of rigidbody physics: Suspension Bridge

https://github.com/ShumWengSang/Perspective-Renderer/tree/main/images/suspensionbridge.mp4

Implementation of rigidbody physics: Stacking of 5 boxes
![Stacking](/images/stacking1.gif)

Implementation of rigidbody physics: Firing into 5 boxes

https://github.com/ShumWengSang/Perspective-Renderer/tree/main/images/boxes.mp4

Implementation of rigidbody physics: Falling boxes

https://github.com/ShumWengSang/Perspective-Renderer/tree/main/images/fallingboxes.mp4

# To Build

Use CMake to build. It should work out of the box as all the libraries are packaged together and built from source.

# To Execute

The default args attempt to load a bunch of factory models available in my own files (they're not uploaded because it would be over 1 GB).
However, passing an argument to the command will cause it to load the obj.

# To do
* Implement a math/geometry/shape library
* Showcase multiple point lights? Or IBL?
* BRDF lighting
* SSAO
* and more to think off!

# Regrets
* I wish I had the discpline to section out seperate scenes as I worked on each feature so I can show them off.
* Unfortunately, I didn't and so people are stuck with seeing the latest work I'm working on

# Credits
* DigiPen Institute of Technology Professor Pushpak Karnick for providing the slides and maths
* [Prospect Renderer](https://github.com/Shimmen/Prospect) - For giving the inspiration and a lot of the technical know-hows to structure my code.
