//
// Created by user on 1/26/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: LightPass.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/26/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_LIGHTPASS_H
#define OPENGLFRAMEWORK_LIGHTPASS_H

#include "ShaderDependant.h"
#include "DirectionalLight.h"

class LightBuffer;

class GBuffer;

class Scene;

class LightPass : public ShaderDependant {
public:
    void Draw(const LightBuffer &lightBuffer, const GBuffer &gBuffer, Scene &scene);

    void ProgramLoaded(GLuint program) override;

    static void RenderGui(DirectionalLight &light, LightBuffer const &lightBuffer);

private:

    GLuint directionalLightProgram{0};
    GLuint directionalLightUniformBuffer{0};
};


#endif //OPENGLFRAMEWORK_LIGHTPASS_H
