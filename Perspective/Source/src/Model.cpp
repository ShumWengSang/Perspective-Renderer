//
// Created by user on 1/25/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: Model.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/25/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "Model.h"
#include "TextureSystem.h"
#include "Logger.h"
#include "Material.h"

int Model::TriangleCount() const { 
    int indices = 0;
  for (int i = 0; i < meshes.size(); ++i) {
      indices += meshes[i].indices.size() / 3;  
  }
  return indices / 3;
}

#ifdef TINYOBJLOADER
void Model::Draw() const {
  if (vao) {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, indexType, nullptr);
  }
}


#elif ASSIMPLOADER
void Model::Draw() const {
  for (unsigned int i = 0; i < meshes.size(); i++) meshes[i].Draw();
}

void Model::loadModel(const std::string & path) {
  Assimp::Importer importer;
  const aiScene* scene =
      importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    LogError("ERROR::ASSIMP::%s", importer.GetErrorString());
    return;
  }
  directory = path.substr(0, path.find_last_of('/'));

  processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
  // process all the node's meshes (if any)
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.emplace_back(processMesh(mesh, scene));
  }
  // then do the same for each of its children
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    glm::vec3 vector;
    vector.x = mesh->mVertices[i].x;
    vector.y = mesh->mVertices[i].y;
    vector.z = mesh->mVertices[i].z;
    vertex.position = vector;

    vector.x = mesh->mNormals[i].x;
    vector.y = mesh->mNormals[i].y;
    vector.z = mesh->mNormals[i].z;
    vertex.normal = vector;

    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        vertex.boneIDs[i] = -1;
        vertex.weights[i] = 0.0f;
    }

    if (mesh->mTextureCoords[0])  // does the mesh contain texture coordinates?
    {
      glm::vec2 vec;
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      vertex.texCoord = vec;
    } else {
      vertex.texCoord = glm::vec2(0.0f, 0.0f);
    }

    // process vertex positions, normals and texture coordinates
    vertices.emplace_back(vertex);
  }
  // process indices
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++)
      indices.emplace_back(face.mIndices[j]);
  }

  // process material
  if (mesh->mMaterialIndex >= 0) {
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<Texture> diffuseMaps = loadMaterialTextures(
        material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<Texture> specularMaps = loadMaterialTextures(
        material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
  }

  // Process Bones
  ExtractBoneWeight(vertices, mesh, scene);

  // Normalize weights
  for (auto& vertex : vertices)
  {
      // Find largest weight
      double largestWeight = 0;
      for(int i = 0 ; i < 4; ++i)
      { 
          largestWeight += (double)vertex.weights[i];
      }
      // Remap it
      for (int i = 0; i < 4; ++i)
      {
          vertex.weights[i] = MyMath::Remap(vertex.weights[i], 0, (float)largestWeight, 0, 1);
      }
  }

  return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat,
                                                 aiTextureType type,
                                                 std::string const & typeName) {
  std::vector<Texture> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);
    Texture texture;
    std::string fileName = std::string(str.C_Str());
    fileName = directory + '/' + fileName;
    texture.id = TextureSystem::getInstance().LoadDataTexture(fileName);
    texture.type = typeName;
    texture.path = str.C_Str();
    textures.emplace_back(texture);
  }
  return textures;
}

void Model::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
    {
        if (vertex.boneIDs[i] < 0)
        {
            vertex.weights[i] = weight;
            vertex.boneIDs[i] = boneID;
            break;
        }
    }
}

void Model::ExtractBoneWeight(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
    // For each bone
    for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
        int boneID = -1;
        // Cache the bones for quick optimization
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            // New bone info add it in
            BoneInfo newBoneInfo;
            newBoneInfo.id = boneCounter;
            glm::mat4 transformation = MyMath::AssimpToMat4(mesh->mBones[boneIndex]->mOffsetMatrix);
            newBoneInfo.offset = MyMath::VQS(transformation);
            boneInfoMap[boneName] = newBoneInfo;
            boneID = boneCounter;
            boneCounter++;
        }
        else
        {
            // Retrieve the bone ID from the map
            boneID = boneInfoMap[boneName].id;
        }
        assert(boneID != -1);
        // Get the number of weights and the weights array
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;
        // For each weight inside the bone
        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            // Find the vertex that is affected by this weight
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId <= vertices.size());
            // The vertex has is given this bone and weight (up till 4)
            SetVertexBoneData(vertices[vertexId], boneID, weight);
        }
    }
}

#endif

