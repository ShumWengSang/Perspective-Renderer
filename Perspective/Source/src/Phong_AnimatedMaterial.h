//
// Created by user on 1/28/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: PowerPlanMaterial.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/28/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_PHONGANIMATED_H
#define OPENGLFRAMEWORK_PHONGANIMATED_H

#include "Material.h"
#include "Uniform.h"

class PhongAnimatedMaterial : public Material {
public:
    PhongAnimatedMaterial();
    ~PhongAnimatedMaterial() = default;

    void ProgramLoaded(GLuint program) override;
    void BindUniforms(Transform& transform, const Transform& prevTransform) const override;

    mutable Uniform<glm::vec3> ambient {"u_mat_ambient", {}};
    mutable Uniform<glm::vec3> diffuse {"u_mat_diffuse", {}};
    mutable Uniform<glm::vec3> specular {"u_mat_specular", {}};
    mutable Uniform<float> Shininess {"u_mat_shininess", {}};

    // Load material data defined by PowerPlantMaterial. Directory must end in "/". Lile
    // "Commons/PowerPlantFiles/"
    void ReadMaterialFromFile(std::string const & directory);
private:
    inline const static std::string fileName = "Sample_MTL_File.mtl";
    GLint modelMatrixLocation = 0;
};


#endif //OPENGLFRAMEWORK_POWERPLANTMATERIAL_H
