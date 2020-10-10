/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: README.txt
 * Purpose: README of CS300 Assigment 1
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M, 
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: RolandShum_CS300_1
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 3rd October 2020
 * End Header --------------------------------------------------------*/

EXTENSION GRANTED BY PROFESSOR PUSHPAK

 Parts completed 
   -- Setting up OpenGL application 
    -- Windows set up
      -- GLApplication.cpp, line 27, main()
    -- OpenGL
      -- OpenGLRendererAPI.cpp line52, OpenGLRendererAPI::Init()
   -- Reading OBJ file
     -- Located in ObjLoader.cpp, LoadObj(), line 53
   -- Vertex and fragment shaders for phong
     -- Shaders
       -- Located in Shaders folder
     -- Child of abstracted Shader class is OpenGLShader (OpenGLShader.h)
     -- Uniforms are sent in SimpleScene_Quad.cpp Render() line 218
     -- View and Projection matrix sent in Renderer.cpp, Renderer::BeginScene() line 39
   -- Scene setup + rotating spheres
     -- Scene is setup in SimpleScene_Quad.cpp, Init(), line 138
     -- VAO and VBO created and set in SimpleScene_Quad.cpp, SetupBuffers() line 73
     -- Sphere generated in ObjLoader.cpp CreateSphere() line 227
   -- Face and vertex normal calculation + display
     -- This is achieved with a geometry shader.
      -- Shader loaded at SimpleScene_Quad.cpp Init() line 144
      -- Shader is called "NormalGenerator"
      -- Shader pass added at SimpleScene_Quad.cpp, SetupBuffers() line 93
   -- Misc ImGui
     -- ImGui
       -- Hold left mouse to scroll change vec3 values, control click to change values directly.
       -- Implemented in SimpleScene_Quad.cpp, preRender(), line 259
       
    -- Compile warnings are attempt to surpress warnings from different compilers.
    
Computer developed: 

             /////////////                roland@pop-os 
         /////////////////////            ------------- 
      ///////*767////////////////         OS: Pop!_OS 20.04 LTS x86_64 
    //////7676767676*//////////////       Host: 3352CTO ThinkPad T430u 
   /////76767//7676767//////////////      Kernel: 5.4.0-7642-generic 
  /////767676///*76767///////////////     Uptime: 2 days, 6 hours, 16 mins 
 ///////767676///76767.///7676*///////    Packages: 2626 (dpkg) 
/////////767676//76767///767676////////   Shell: bash 5.0.17 
//////////76767676767////76767/////////   Resolution: 1366x768 
///////////76767676//////7676//////////   DE: Plasma 
////////////,7676,///////767///////////   WM: KWin 
/////////////*7676///////76////////////   Theme: Breeze [Plasma], Breeze [GTK2/3] 
///////////////7676////////////////////   Icons: breeze [Plasma], breeze [GTK2/3] 
 ///////////////7676///767////////////    Terminal: konsole 
  //////////////////////'////////////     CPU: Intel i7-3537U (4) @ 3.100GHz 
   //////.7676767676767676767,//////      GPU: Intel 3rd Gen Core processor Graphics Controller 
    /////767676767676767676767/////       GPU: NVIDIA GeForce 620M
      ///////////////////////////         Memory: 5774MiB / 15857MiB 
         /////////////////////
             /////////////                                        
                                 
OpenGL vendor string: NVIDIA Corporation
OpenGL renderer string: GeForce GT 620M/PCIe/SSE2
OpenGL core profile version string: 4.6.0 NVIDIA 390.138
OpenGL core profile shading language version string: 4.60 NVIDIA
OpenGL core profile context flags: (none)
OpenGL core profile profile mask: core profile
OpenGL core profile extensions:
OpenGL version string: 4.6.0 NVIDIA 390.138
OpenGL shading language version string: 4.60 NVIDIA
OpenGL context flags: (none)
OpenGL profile mask: (none)
OpenGL extensions:
OpenGL ES profile version string: OpenGL ES 3.2 NVIDIA 390.138
OpenGL ES profile shading language version string: OpenGL ES GLSL ES 3.20
OpenGL ES profile extensions:
              


Hours Spent per week: 6 hours
Additional Information: Abstracted Renderer from openGL code. Used geometry shader for line rendering.
Please use Linux and install glfw3, glm, and glew

sudo apt-get install libglfw3-dev libglm-dev libglew-dev

Further information below.

 ------------------------------------------------------------------------------------------------

 
 Summary:
   The project consists of a framework written on on top of the given scene framework. 
   Inspired by the Hazel engine by Cherno, the framework seeks to bring OOP to the 
   OpenGL objects. There is an abstraction layer known as the Renderer, and then below
   that we have our OpenGL*Object*. 
    
   The project was developed in Linux, G++. While theoretically it can build and compile
   on other platforms and other compilers, it has not been tested.
    
Instructions:
  The project is compiled with Cmake. To compile, go to Build and input "cmake .." and then 
  call the generated make file. This should generate an exe called "SimpleScene_OGL4" 
  in the Build folder. Feel free to use any other supported IDE for CMake.
  
  To run the project, just run the exe. This will load the cube2.obj.
  To facilitate grading, you may pass a c-string from the command line to the exe and 
  it will load that obj file.
    IE: SimpleScene_OGL4 cube.obj

    
Useful Information:
  SimpleScene_Quad::Init() is where everything is kickstarted.
  
  OpenGLRendererAPI::Init() is where OpenGL settings is set at first.
  
  OpenGLRendererAPI::DrawIndexed() is where the draw call is called.
  
  OpenGLBuffer.cpp is where all the Vertex Buffer and Vertex Array code exists. 
  Of note is OpenGLVertexArray::AddVertexBuffer(), which turns on the attributes 
  based on the given layout.
  
  Shaders support geometry shader! This is used in generating the face vertex lines and the 
  vertex normal lines!
    
  OBJ loader is located in ObjLoader.cpp line 53.
  Procedural sphere geneator is located in ObjLoader.cpp line 223
  
Simple Directory Diagram:

root
    |
    |-- README.txt
    |
    Common
    |
    |-- Professor Pushpak files
    |
    |
    1.0 Simple Scene
    |
    |-- Build - Use CMake here (all the build dumbs are here)
    |
    |-- ImGui - All ImGui files
    |
    |-- OpenGL - All Opengl related code are abstracted here.
    |     |
    |     |- Take note that the Vertex Array class is bundled with the Vertex Buffer class.
    |
    |-- Renderer - Base classes that abstract rendering code lives here.
    |
    |-- Shaders - All the shaders used live here
    |
    |----- Scene files and misc files
    
Shaders:
    Phong Shader: We use a phong shader with a global light direction to create the illusion of light on our objects.
    
    Line Shader: The sphere line uses a standard shader that puts the object into NDC with a given color and renders it.
    
    Normal Generating Shader: Generated GL_LINES that represents the face normals and vertex normals.
  
  
