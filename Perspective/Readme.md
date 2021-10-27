# Advanced Animation CS460 Assignment 1
### Student: Roland Shum
### Professor: Prof Xin Li

# Assignment Objectives
(1) Modelling:
* Loading of .fbx model 
  * Imported ASSIMP library
  * https://github.com/assimp/assimp
  * Library has known issues with fbx files, so untested models may break
* Imported animation using .fbx files 
  * Using ASSIMP library again
* Used node tree to graph bones
  * Used my own implementation of Quaternion and VQS
  * Used VQS concatenation to transform spaces
* Loaded file *alien.fbx*

(2) Animation:
* Read key frames and interpolate between keyframes for animation
* Implemented Lerp, Slerp, ELerp
  * Demonstrated and used in exe
* Implemented iSlerp and iVQS
  * iSlerp is used and demonstrated in build
  * iVQS proved very troublesome to showcase as it requires preprocessing of each frame 
    * ASSIMP gives a [position, timestamp], [rotation, timestamp], and [scale, timestamp] for each bone
    * To properly use iVQS I would need to
      * At each timestamp regardless of position, rotation or scale
      * Record the [position, rotation or scale]
      * Interpolate the other two elements to find the right values for them
      * Insert into keyframe
    * This proved too long to do as my animation does not give a position, rotation, and scale at every time frame.
      * To solve this, make animation with position, rotation, and scale at every time frame
      * Or post process the animation to generate such frames
        * But this runs the risk of going against the animators vision as its not guranteed they want to lerp or slerp
* Incremental approaches run at 60 FPS, while others run at a variadic rate

# How to Build:
* **Run prepackaged file "RunCMake.bat" **which will 
  * Run the packaged portable cmake
    * cmake will generate the visual studio solution
  * Open VS solution
  * Build + Run
* This should have generated a vsbuild folder
  * Inside the folder is the Visual Studio Solution
  * **Build and run project "Perspective"**

# Instructions / Controls:
* **[WASD] camera movement**
* **[Hold Right Mouse button + Drag] to rotate camera**
* **Scroll to zoom **
* Rendering speed of ~110 FPS on my laptop
* Modify ImGui GUI at will to make things nice and simple
  * There are GUI controls in the ImGui meant for showcasing different lerp
  * GUI controls for selecting which animation to play
  * GUI controls to determine whether to render bones


# High Level Explanation
* Framework modified from previous CS class
  * That is why it uses a CMake pipeline
  * Has deferred and forward rendering for OpenGL
* Relevant files for grading:
  * Project: Perspective
    * CS460AssignmentOne.cpp 
      * Main entry point to the scene
    * Bone.cpp
      * Abstracts a bone 
      * **Interpolation functions for bones are called here**
    * Model.cpp
      * Holds multiple meshes
      * Loads meshes from assimp
      * Loads vertex data
    * Quaternions.cpp
      * **My implementation of quaternions**
      * **My implementation of VQS**
    * Main.cpp
      * Holds test cases for VQS and quaternion implementation
    * Animation.cpp
      * Holds a specific animation reel loaded from ASSIMP
    * Animator.cpp
      * Holds a pointer to the current animation playing
      * Updates that animation 
    * MyMath.cpp
      * Holds implemntation for 
        * Lerp
        * Slerp
          * Slight modification to slerp 
            * When alpha < 0.0f
              * It swaps around one of the quaternions for correct interpolation
            * When alpha is close to 1.0f
              * Use Lerp to avoid side effects of flipping
        * iLerp
        * iSlerp
        * eLerp
  * Shaders
    * shader/material/Phong_Animated.vert.glsl
      * Skinning
      * VQS sent to shaders via UBO
      * VQS conversion to mat4 

# Developed on
Windows:

    OS: Windows NT 10.0.18362.0

    Host: GIGABYTE AERO 15Wv8

    CPU: Intel(R) Core(TM) i7-8750H CPU @ 2.20GHz
          
    GPU: Intel(R) UHD Graphics 630 NVIDIA GeForce GTX 1060

    OpenGL version: 4.6 NVIDIA 451.67

    Visual Studio 2019 Preview 

    Windows SDK version : 10.0.18362.0







