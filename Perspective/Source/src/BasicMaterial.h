//
// Created by user on 1/25/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: BasicMaterial.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/25/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_BASICMATERIAL_H
#define OPENGLFRAMEWORK_BASICMATERIAL_H

#include "Material.h"

class BasicMaterial : public Material {
public:
    BasicMaterial();

    ~BasicMaterial() = default;

    glm::vec3 baseColor;
    float roughness;
    float metallic;

    void ProgramLoaded(GLuint program) override;

    void BindUniforms(Transform &transform, const Transform &prevTransform) const override;

private:

    GLint baseColorLocation;
    GLint roughnessLocation;
    GLint metallicLocation;

    GLint modelMatrixLocation;
    GLint prevModelMatrixLocation;
    GLint normalMatrixLocation;
};


#endif //OPENGLFRAMEWORK_BASICMATERIAL_H
