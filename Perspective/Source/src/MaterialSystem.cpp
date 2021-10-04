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

#pragma region Internal API

typedef struct {
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texcoords;
    std::vector<int> v_indices;
    std::vector<int> vn_indices;
    std::vector<int> vt_indices;

    std::vector<tinyobj::material_t> materials;

} MyMesh;

static void usemtl_cb(void *user_data, const char *name, int material_idx) {

    MyMesh *mesh = reinterpret_cast<MyMesh *>(user_data);
    if ((material_idx > -1) && (material_idx < mesh->materials.size())) {
        printf("usemtl. material id = %d(name = %s)\n", material_idx,
               mesh->materials[material_idx].name.c_str());
    } else {
        printf("usemtl. name = %s\n", name);
    }
}

static void mtllib_cb(void *user_data, const tinyobj::material_t *materials,
        int num_materials) {
//    MyMesh *mesh = reinterpret_cast<MyMesh *>(user_data);
    printf("mtllib. # of materials = %d\n", num_materials);

//    for (int i = 0; i < num_materials; i++) {
//        mesh->materials.push_back(materials[i]);
//    }
}
#pragma endregion

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

    //if (hasDiffuseTex && hasNormalMap && hasRoughnessMap)
    //{
    //    auto mat = new CompleteMaterial();
    //    TextureSystem& textureSystem = TextureSystem::getInstance();

    //    mat->baseColorTexture = textureSystem.LoadHdrImage(baseDirectory + materialDescription.diffuse_texname);
    //    mat->normalMap = textureSystem.LoadDataTexture(baseDirectory + materialDescription.normal_texname);
    //    mat->roughnessMap = textureSystem.LoadDataTexture(baseDirectory + materialDescription.roughness_texname);

    //    if (hasMetallicMap)
    //    {
    //        mat->metallicMap = textureSystem.LoadDataTexture(baseDirectory + materialDescription.metallic_texname);
    //    }
    //    else
    //    {
    //        // Assume not metal if no map is specified
    //        mat->metallicMap = textureSystem.CreatePlaceholder(0, 0, 0, 0);
    //    }

    //    material = mat;
    //}
    //else
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

const tinyobj::material_t& MaterialSystem::CreateMaterial(const std::string &fileDirectory) {
    std::ifstream ifs(fileDirectory);
    std::string err;
    tinyobj::LoadMtl(&materialMap, &materials, &ifs, &err);
    return materials.back();
}
