# Advanced Animation CS460 Assignment 4
### Student: Roland Shum
### Professor: Prof Xin Li

# Assignment Objectives
(1) Object Modeling:
  * Option A completed with Suspension Bridge scenario
  * Interaction with environment
    * Press 'SPACEBAR' to fire box in front of camera
  
(2) Physically based Animation
    * Robust implementation of physics engine
      * Semi Implicit Euler Integration
      * Runge-Kutta 2nd Order
      * Runge-Kutta 4th Order (Extra Credit)
    * Implemented Joint Constraint Solver
    * Implemented Contact Constraint and Friction Solver 
    * Stable and realistic behavior
    
(3) Visualization
    * Five scenarios implemented to demonstrate physics engine 
    * Rendered contact points + cubes + joints

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
* Spacebar to fire 
* Rendering speed of ~140 FPS on my laptop
* Modify ImGui GUI at will to make things nice and simple


# High Level Explanation
* Framework modified from previous CS class
  * That is why it uses a CMake pipeline
  * Has deferred and forward rendering for OpenGL
* Relevant files for grading:
  * Project: Perspective
    * Contact.cpp
        * Contains information relevant to the collision between two objects
        * Uses SAT to detect collision between two objects
        * SAT to give axis of collision 
        * SAT to give penetration distance of collision
        * GJK to give point of collision
        * Builds a Contact based on these information
    * Physics.cpp
        * Physics loop
        * External forces integrated to velocity
        * Velocity integrated to position/rotation
        * Implemented RK2, RK4, Semi-Euler integration
    * Rigidbody.cpp
        * Contains all the relevant information for a rigidbody
    * rbSolver.cpp
        * Contact Solver
        * Applies impulses to 'correct' velocity and position
    * Joint.cpp
        * Joint Solver

# Developed on
Windows:

    OS: Windows NT 10.0.18362.0

    Host: GIGABYTE AERO 15Wv8

    CPU: Intel(R) Core(TM) i7-8750H CPU @ 2.20GHz
          
    GPU: Intel(R) UHD Graphics 630 NVIDIA GeForce GTX 1060

    OpenGL version: 4.6 NVIDIA 451.67

    Visual Studio 2019 Preview 

    Windows SDK version : 10.0.18362.0







