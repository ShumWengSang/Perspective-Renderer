/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: README.txt
 * Purpose: README of CS300 Assigment 2
 * Language: C++, G++
 * Platform: Linux:
 *              g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M, 
 *              OpenGL version string: 4.6.0 NVIDIA 390.138
 *           Windows:
 *              OS: Windows NT 10.0.18362.0
 *              Host: GIGABYTE AERO 15WV8
 *              CPU: Intel(R) Core(TM) i7-8750H CPU @ 2.20GHz
 *              GPU: Intel(R) UHD Graphics 630 NVIDIA GeForce GTX 1060
 *              OpenGL version: 4.6 NVIDIA 451.67
 * Project: RolandShum_CS300_2
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 11th November 2020
 * End Header --------------------------------------------------------*/


 Parts completed 
   -- All parts of assignment completed
    -- Texture loading
      -- Texture.cpp line52, Texture::CreateTexture2DFromFile()
      -- Uses STB image library
      -- Loads PNG converted from given PPM using Paint.NET (thus artifacts may be seen on texture)
   -- Shaders
     -- Uses UBOs to transfer light data to shaders
     -- Phong Lighting
        -- Shaders/Assignment2/PhongLighting vert/frag
     -- Phong Shader
        -- Shaders/Assignment2/PhongShader
     -- BlinnPhong
        -- Shaders/Assignment2/BlinnPhong frag (vert uses the same as PhongShader.vert)
     -- Reload of shaders done under ImGui->Shader Library->Pipelines->#SHADERNAME#->Reload
     -- You can specify which shader to be using to render the mesh in realtime using ImGui
        -- ImGui -> Shader to use? -> (Toggle between shaders)
     -- Implemented multiple lights.
        -- ImGui -> Light Settings
   -- UV Mapping
        -- CPU and GPU
        -- Both normal and positional generation.
        -- ImGui -> UV Generation Mode (if CPU, you have to click the reload buttons)
            -- "Use Normals" means to use normals or position to generate UV.
            -- CPU Generate -> (New header shows up for CPU)
            -- GPU Generate -> (New header shows up for GPU)
        
   -- Gui Features implemented
        -- Number of lights (at top of ImGui interface)
        -- Light Type (light settings -> light %number -> toggle
        -- Pause/start light rotation
        -- Multiple scenarios for quick grading.
       
    -- Compile warnings are attempt to surpress warnings from different compilers.

    
Computers developed: 

Linux:

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


Windows:
 user@DESKTOP-VGHHFDO

 OS: Windows NT 10.0.18362.0
 Host: GIGABYTE AERO 15WV8
 Uptime: 2 days 55 minutes
 Packages:
 PowerShell: PowerShell v5.1.18362.1110
 CPU: Intel(R) Core(TM) i7-8750H CPU @ 2.20GHz
 GPU: Intel(R) UHD Graphics 630 NVIDIA GeForce GTX 1060
 OpenGL version: 4.6 NVIDIA 451.67
 Geforce Drivers 451.67 7-5-2020 Version 86.6.44.0.58
              


Hours Spent: 48 hours
Additional Information: Used geometry shader for line rendering. Used OpenGL 4.6 DSA paradigm.
Please use Linux and install glfw3, glm, and glew

sudo apt-get install libglfw3-dev libglm-dev libglew-dev

Further information below.

 ------------------------------------------------------------------------------------------------

 
 Summary:
   The entire framework was rewritten to use OpenGL 4.6 DSA methods, as well as having a low abstraction
   paradigm. So most things can be seen in the Quad_Scene::SetupBuffers() function.
   The paradigm modelled here is that the user controls the creation of opengl objects, but do not have
   to worry about destruction of opengl objects.
   
   Heavily inspired by: https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions
    
   The project was developed in Linux, G++. Also tested and usable in Windows with CLion
   and vcpkg, using MSVC compiler.
    
Instructions:
  The project is compiled with Cmake. To compile, go to Build and input "cmake .." and then 
  call the generated make file. This should generate an exe called "SimpleScene_OGL4" 
  in the Build folder. Feel free to use any other supported IDE for CMake.
  
  To run the project, just run the exe. This will load the default sphere.obj.
  To facilitate grading, you may pass a c-string from the command line to the exe and 
  it will load that obj file.
    IE: SimpleScene_OGL4 cube2.obj

  If using MSVC on Windows, the CMake is coded so that it works with vcpkg. You have to link
  the vcpkg to the Cmake with -DCMAKE_TOOLCHAIN_FILE="C:/vcpkg-2020.07/scripts/buildsystems/vcpkg.cmake"
  This was used with Clion. Settings->Build,Execution,Deployment -> CMake -> CMake options:
  
  Use the scenarios for quick preview of lights that are setup already.
    
Useful Information:
  You can view SimpleScene_Quad::SetupBuffers() is where most of the work is done.
  We are reusing the PhongShader.vert as our blinn-phong vertex shader.
  This is all done with the OpenGL Pipeline binding.
  
  Shaders support geometry shader! This is used in generating the face vertex lines and the 
  vertex normal lines!

  
Simple Directory Diagram:

root
    |
    |-- README.txt
    |
    Common
    |
    |-- Models
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
    
  
