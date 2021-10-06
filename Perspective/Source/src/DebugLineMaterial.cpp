//
// Created by user on 1/30/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: DebugLineMaterial.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/30/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "DebugLineMaterial.h"
#include "TransformSystem.h"
#include "ShaderSystem.h"

void DebugLineMaterial::ProgramLoaded(GLuint program) {
    this->program = program;
    if(program)
    {
        modelMatrixLocation = glGetUniformLocation(program, "u_world_from_local");
        colorLocation = glGetUniformLocation(program, "u_color");
    }
}

void DebugLineMaterial::BindUniforms(Transform &transform, const Transform &prevTransform) const {
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(transform.matrix));
}

void DebugLineMaterial::BindUniforms(glm::mat4 ModelMatrix) const {
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(ModelMatrix));
}

void DebugLineMaterial::BindColor(const glm::vec4& color) const
{
    glUniform4fv(colorLocation, 1, glm::value_ptr(color));
}

DebugLineMaterial::DebugLineMaterial()
{
    ShaderSystem::getInstance().AddProgram("material/debugLine", this);
}
