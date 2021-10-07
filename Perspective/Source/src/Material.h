//
// Created by user on 1/25/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: Material.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/25/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_MATERIAL_H
#define OPENGLFRAMEWORK_MATERIAL_H

#include "ShaderDependant.h"

class Transform;

class Material : public ShaderDependant {
public:
    bool opaque = true;
    bool cullBackface = true;

    GLuint program = 0;

    virtual void ProgramLoaded(GLuint program) = 0;

    virtual void BindUniforms(Transform &transform, const Transform &prevTransform) const = 0;
};


#endif //OPENGLFRAMEWORK_MATERIAL_H
