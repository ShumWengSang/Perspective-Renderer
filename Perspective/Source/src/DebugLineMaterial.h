//
// Created by user on 1/30/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: DebugLineMaterial.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/30/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_DEBUGLINEMATERIAL_H
#define OPENGLFRAMEWORK_DEBUGLINEMATERIAL_H

#include "Material.h"
class DebugLineMaterial : public Material {
public:
    void ProgramLoaded(GLuint program) override;
    void BindUniforms(Transform& transform, const Transform& prevTransform) const override;
    void BindUniforms(glm::mat4 ModelMatrix) const;

    void BindColor(const glm::vec4& color) const;
    DebugLineMaterial();
private:
    GLint modelMatrixLocation = 0;
    GLint colorLocation = 0;
};


#endif //OPENGLFRAMEWORK_DEBUGLINEMATERIAL_H
