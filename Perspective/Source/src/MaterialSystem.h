//
// Created by user on 1/25/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: MaterialSystem.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/25/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_MATERIALSYSTEM_H
#define OPENGLFRAMEWORK_MATERIALSYSTEM_H

class Material;

class MaterialSystem {
public:
    static MaterialSystem &getInstance() {
        static MaterialSystem instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    MaterialSystem(MaterialSystem const &) = delete;

    void operator=(MaterialSystem const &) = delete;

private:
    MaterialSystem() = default;

public:
    Material *CreateMaterial(const tinyobj::material_t &materialDescription, const std::string &baseDirectory);

    const tinyobj::material_t &CreateMaterial(std::string const &name);

    void ManageMaterial(Material *material);

    void Destroy();

private:
    std::vector<Material *> managedMaterials{};

    std::vector<tinyobj::material_t> materials;
    std::map<std::string, int> materialMap;
};


#endif //OPENGLFRAMEWORK_MATERIALSYSTEM_H
