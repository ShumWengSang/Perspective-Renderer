//
// Created by user on 1/27/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: GeometryPass.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/27/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_GEOMETRYPASS_H
#define OPENGLFRAMEWORK_GEOMETRYPASS_H
#include "ShaderDependant.h"

class GBuffer;
class Scene;

class GeometryPass : public ShaderDependant {
public:
    void Draw(const GBuffer& gBuffer, Scene& scene);
    void ProgramLoaded(GLuint program) override;

    bool performDepthPrepass = false;
    bool wireframeRendering = false;
private:
    GLuint depthOnlyProgram = 0;
    GLint modelMatrixLocation;
};


#endif //OPENGLFRAMEWORK_GEOMETRYPASS_H
