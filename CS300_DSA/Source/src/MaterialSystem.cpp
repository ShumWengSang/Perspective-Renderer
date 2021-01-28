//
// Created by user on 1/25/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: MaterialSystem.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/25/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "MaterialSystem.h"
#include "Material.h"
#include "TextureSystem.h"
#include "BasicMaterial.h"

Material *
MaterialSystem::CreateMaterial(const tinyobj::material_t &materialDescription, const std::string &baseDirectory) {
    //
    // TODO: Create materials properly! Don't assume a certain material, do the best possible material given the supplied properties
    //
    Material *material;

    bool hasDiffuseTex = !materialDescription.diffuse_texname.empty();
    bool hasNormalMap = !materialDescription.normal_texname.empty();
    bool hasRoughnessMap = !materialDescription.roughness_texname.empty();
    bool hasMetallicMap = !materialDescription.metallic_texname.empty();

/*    if (hasDiffuseTex && hasNormalMap && hasRoughnessMap)
    {
        auto mat = new CompleteMaterial();
        TextureSystem& textureSystem = TextureSystem::getInstance();

        mat->baseColorTexture = textureSystem.LoadLdrImage(baseDirectory + materialDescription.diffuse_texname);
        mat->normalMap = textureSystem.LoadDataTexture(baseDirectory + materialDescription.normal_texname);
        mat->roughnessMap = textureSystem.LoadDataTexture(baseDirectory + materialDescription.roughness_texname);

        if (hasMetallicMap)
        {
            mat->metallicMap = textureSystem.LoadDataTexture(baseDirectory + materialDescription.metallic_texname);
        }
        else
        {
            // Assume not metal if no map is specified
            mat->metallicMap = textureSystem.CreatePlaceholder(0, 0, 0, 0);
        }

        material = mat;
    }
    else*/
    {
        auto mat = new BasicMaterial();

        mat->baseColor = glm::vec3(
                materialDescription.diffuse[0],
                materialDescription.diffuse[1],
                materialDescription.diffuse[2]
        );
        mat->roughness = materialDescription.roughness;
        mat->metallic = materialDescription.metallic;

        material = mat;
    }

    managedMaterials.push_back(material);
    return material;
}

void MaterialSystem::ManageMaterial(Material *material) {
    managedMaterials.push_back(material);
}

void MaterialSystem::Destroy() {
    for (Material *material : managedMaterials)
    {
        delete material;
    }
}
