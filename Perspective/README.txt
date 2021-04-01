/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: README.txt
 * Purpose: README of CS350 Assigment 3
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
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: April 1st 2021
 * End Header --------------------------------------------------------*/


Parts completed (100%)
    -- Added functionality to stop deferred rendering of object
        - ImGui 
            - Geometry Pass
                - Draw Obj checkbox
    -- Assignment Three Options
        -- Oct Tree
            - Adaptive top-down oct tree
            - Only leaf nodes contain triangles
            - Triangle that straddles boundaryt are split
        -- BSP Tree
            - Top- down BSP
                - Every leaf nodes contains geometry, and internal nodes only contain information about the split plane
                - Straddling triangles are split 
            - Choosing a split plane
                - Flip flops between randomly finding a plane and getting a random triangle's plane to split on
                    - Each iteration runs this "x" times based on the Plane Split Sampling and finds the best plane to split on
                    - "Random Split Lower Bound" gives a fast case for the algorithm to escape once its found a plane that is "good enough that we don't need to try other planes"
                - Experimentations with splitting on [X,Y,Z] axis has shown that it will degrade performance
        -- Optimising BSP Tree Creation
            - JSON Serializing and deserialization
                - For both octtree and BSP
                - The source code by itself loads only Section 10
                    - Can be changed in AssignmentThree.cpp line 100
                    - BSP and OctTree generated for this model as well.
        -- Displaying the Tree
                - Change the number of triangles a node can hold (affects both Oct Tree and BSP)
                - Added a max depth so that generation of trees can stop.
                - Visualizing (only available if octtree or bsp were generated/loaded)
                    - You can specify the level of oct tree to view
                        - 0 specifies to render all levels
                        - This applies to both Bounding Volume and node wireframe
                    - You can specify if you want to render AABB bounding boxes or the lines (that make the triangles) for octree
                    - There is a risk of "drawing too many things" that would bring down the program
                    - BSP allows you to render the lines that make up the triangles
        
To Compile:
    -- ALL LIBRARIES COME WITH SOURCE FILE
    -- Use CMake 3.16 or above.
    -- Tested with MSVC Cmake on Windows,
    -- Tested with Pop_OS g++
    -- There is non-crossplatform code that may fail on other distros of Linux, but works on PopOS at least.
    -- Just load the cmake file and build.
    
    IF CLION ON WINDOWS
        - Just run it on clion and it'll all work.
    IF VS ON WINDOWS
        - Create vsbuild directory and run RunCMake.bat
        - Creates a VS Solution that has been tested
    IF Linux
        - You know what to do
    

[IMPORTANT] To Execute :
    -- Accepts command line args of the path of a model.
        -- For example, suppose sphere.obj exists in Common/models/sphere.obj
        -- You would pass OpenGLFramework.exe Common/models/sphere.obj
    -- No arguments make it load all sections of powerplant 
        -- Expects the directory to be structured as below
        -- Relevant code at CS350.cpp line 74
            -- Reads the SectionX.txt files,
                -- Assumes base directory at SectionX.txt files, read the corresponding obj.
                -- Loaded on another thread, so no need to wait.
                
When Running Application:
    WASD to move camera
        -- If camera not moving, make sure to focus on application (You're probably focused on the ImGui)
    Drag right mouse click to rotate camera
    Models are loaded on another thread, so TAs don't have to worry about changing source code or command line args to test.
    
    [NOTE] The ImGui docking windows are probably right on top of each other.
        -- To view the GBuffer, open up the GBuffer in ImGui.
        -- Recommended to move Console / Log to bottom
    
    [IMPORTANT]
    -- Directory it expects
        * CMake Project
        |
        |
        |---- Source
        |
        |
        |---- shaders
        |
        |
        |---- Common
           |
           |
           |----- models
           |
           |----- textures
           |
           |----- PowerPlantFiles 
               |
               |
               |-- Sample_MTL_File.mtl
               |
               |-- Section1.txt
               |
               |--- ppsection 1
                  |
                  |--- part_a
                    |
                    | -- g0.obj
                    etc
              etc


    
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
              


Hours Spent: 35 hours


Further information below.

 ------------------------------------------------------------------------------------------------

 
 Summary:
   The entire framework was rewritten to use OpenGL 4.6 DSA methods, as well as having a low abstraction
   paradigm. The ideas behind each abstraction was to keep locality of variables.
   The paradigm modelled here is that the user controls the creation of opengl objects, but do not have
   to worry about destruction of opengl objects.
   
   Heavily inspired by: 
    - https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions
    - https://github.com/Shimmen/Prospect
    

