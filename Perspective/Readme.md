# Advanced Animation CS460 Assignment 3
### Student: Roland Shum
### Professor: Prof Xin Li

# Assignment Objectives
(1) Object Modeling:
  * Default set to a depth of 5, can handle depth of 20 (since animation has ~20 nodes in a chain)
  * Attempts to find a solution to the target (returns true or false)
  * Implemented moving along path towards target, then change state to do IK animation
(2) Inverse Kinematics
    * CCD implemented
      * Will find a solution for end effector to reach target within x iterations.
    * Motions generated creating a new Animation frame that slerps rotation.
      * Motion is smooth
      * No constraints implemented
      * No animation blending implemented, so it is jumpy sometimes
    * Enforced Priority implemented 
      * For every node from tip to root
        * Once you modify the current node, modify the 3 nodes ahead of current node again.
    * Alien will move in path to the green ball, and when it reaches it will move its head down to it.

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
* Q and E to accelerate up and down
* **Scroll to zoom **
* Rendering speed of ~110 FPS on my laptop
* Modify ImGui GUI at will to make things nice and simple
  * Restart button to restart animation
    * This updates the Curve to go to the target point 
  * 'Inverse Kinematics' 
    * You can see the bones the IK is affecting
    * "Show IK Anim Solution" 
    * Can modify the number of iterations per frame to find a solution
    * Can modify the threshold (how close enough to the target) to be considered solved
    * Can modify the number of bones to affect (from end effector)


# High Level Explanation
* Framework modified from previous CS class
  * That is why it uses a CMake pipeline
  * Has deferred and forward rendering for OpenGL
* Relevant files for grading:
  * Project: Perspective
    * CS460AssignmentOne.cpp 
      * Main entry point to the scene
      * Implemented animation state machine to determine what the animation should do
      * When the running animation reaches the last point, the IK will attempt to solve 
    * Animator.h
      * Retrieves the current animation's bones that are chained
      * Can manually apply the IK chain to see end result of IK 
    * IKSolver.h/.cpp
      * CCD solver implemented
      * Puts nodes in World Transform to find rotation 
      * No constraints implemented 
      * Enforced Priority implemented

# Developed on
Windows:

    OS: Windows NT 10.0.18362.0

    Host: GIGABYTE AERO 15Wv8

    CPU: Intel(R) Core(TM) i7-8750H CPU @ 2.20GHz
          
    GPU: Intel(R) UHD Graphics 630 NVIDIA GeForce GTX 1060

    OpenGL version: 4.6 NVIDIA 451.67

    Visual Studio 2019 Preview 

    Windows SDK version : 10.0.18362.0







