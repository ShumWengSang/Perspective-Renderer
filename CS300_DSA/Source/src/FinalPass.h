//
// Created by user on 1/27/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: FinalPass.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/27/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_FINALPASS_H
#define OPENGLFRAMEWORK_FINALPASS_H
#include "ShaderDependant.h"
#include "Uniform.h"
// This is just here to make sure people know that
// the final pass is dependant on what assignment
// I'm working on.

class AssignmentOne;

template<typename T>
class FinalPass : public ShaderDependant{
public:

};

class GBuffer;
class LightBuffer;
class Scene;

template<>
class FinalPass<AssignmentOne> : public ShaderDependant{
public:
    void Draw(const GBuffer &gBuffer, const LightBuffer& lightBuffer, const Scene& scene);
    void ProgramLoaded(GLuint program) override;
private:

    Uniform<int> copyDepthBuffer {"copyDepthBuffer", 1};
    GLuint finalProgram;
};


#endif //OPENGLFRAMEWORK_FINALPASS_H
