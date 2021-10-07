//
// Created by user on 1/25/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: BasicMaterial.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/25/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "BasicMaterial.h"
#include "TransformSystem.h"
#include "ShaderSystem.h"

BasicMaterial::BasicMaterial() {
    ShaderSystem &shaderSystem = ShaderSystem::getInstance();
    shaderSystem.AddProgram("material/basic", this);
}

void BasicMaterial::ProgramLoaded(GLuint program) {
    this->program = program;
    if (program) {
        modelMatrixLocation = glGetUniformLocation(program, "u_world_from_local");

        baseColorLocation = glGetUniformLocation(program, "u_base_color");
        roughnessLocation = glGetUniformLocation(program, "u_roughness");
        metallicLocation = glGetUniformLocation(program, "u_metallic");
    }
}

void BasicMaterial::BindUniforms(Transform &transform, const Transform &prevTransform) const {
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(transform.matrix));

    glUniform3fv(baseColorLocation, 1, glm::value_ptr(baseColor));
    glUniform1f(roughnessLocation, roughness);
    glUniform1f(metallicLocation, metallic);
}
