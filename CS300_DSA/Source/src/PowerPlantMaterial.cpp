//
// Created by user on 1/28/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: PowerPlanMaterial.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/28/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "PowerPlantMaterial.h"
#include "ShaderSystem.h"
#include "TransformSystem.h"
#include "MaterialSystem.h"

void PowerPlantMaterial::ProgramLoaded(GLuint program) {
    this->program = program;
    if (program)
    {
        modelMatrixLocation = glGetUniformLocation(program, "u_world_from_local");
    }
}

void PowerPlantMaterial::BindUniforms(Transform &transform, const Transform &prevTransform) const {
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(transform.matrix));
    ambient.UpdateUniformIfNeeded(program);
    diffuse.UpdateUniformIfNeeded(program);
    specular.UpdateUniformIfNeeded(program);
    Shininess.UpdateUniformIfNeeded(program);
}

PowerPlantMaterial::PowerPlantMaterial() {
    ShaderSystem::getInstance().AddProgram("material/PowerPlant", this);
}

void PowerPlantMaterial::ReadMaterialFromFile(const std::string &directory) {
    auto mat = MaterialSystem::getInstance().CreateMaterial(directory + fileName);
    Shininess.value = mat.shininess;
    ambient.value = glm::make_vec3(mat.ambient);
    diffuse.value = glm::make_vec3(mat.diffuse);
    specular.value = glm::make_vec3(mat.specular);
}
