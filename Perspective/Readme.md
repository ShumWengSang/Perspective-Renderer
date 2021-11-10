# Advanced Animation CS460 Assignment 2
### Student: Roland Shum
### Professor: Prof Xin Li

# Assignment Objectives
(1) Path Interpolation:
* Implemented bezier path
    * ImGui for adding and moving points to move through
    * Generates control points for every pair of given points to move through
    * Generates lookup table based on arclength to t, where t is the bezier cubic interpolation value
    * Calculates arclength of entire curve path and reparameterizes all the individual bezier segment tables
(2) Arc Length Calculation
    * Normalized arclength calculate + above impemented
    * All implemented
(3) Speed and Orientation Control
    * Implemented ease-in and ease-out distance time function with a constant speed in the middle section
    * Speed tied to object animation
    * Control the orientation of the model using Forward Mode,
        * Average of 5 points ahead of the model taken
        * At the end of the curve, use a lerp between two points near the end

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
* Q and E to accelerate up and down
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
    * BezierCurve.h
      * Implements the concatenation of multiple bezier segments to form a curve
        * Given a set of points,
        * constructs a curve segment out of every two points, and generates an additional two control points
        * Also generates lookup tables for (distance -> t)
        * Calculates the total distance of the bezier curve, and reparameterize each lookup table using distance 
      * Interpolate goes from 0 to 1, based on arclength interpolation
      * Given a t, maps to a given curve segment
        * In the curve segment, it then calculates the distance it needs to travel
        * and looks up the two closest distance available in loopup (distance -> t) table 
        * and interpolates between the ts of those two distances
    * EaseInOutVelocity.h
        * Implemented a parabolic ease-in ease-out
        * Given a t1 and t2, calculate a velocity
    * CS460AssignmentOne.cpp
        * Animation speed control
            * Modifies the animation playing speed
        * Orientation control
            * Implemented forward viewing by taking the average of the forward 5 points and looking towards it

# Developed on
Windows:

    OS: Windows NT 10.0.18362.0

    Host: GIGABYTE AERO 15Wv8

    CPU: Intel(R) Core(TM) i7-8750H CPU @ 2.20GHz
          
    GPU: Intel(R) UHD Graphics 630 NVIDIA GeForce GTX 1060

    OpenGL version: 4.6 NVIDIA 451.67

    Visual Studio 2019 Preview 

    Windows SDK version : 10.0.18362.0







