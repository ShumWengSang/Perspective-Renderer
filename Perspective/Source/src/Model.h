//
// Created by user on 1/25/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: Model.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/25/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_MODEL_H
#define OPENGLFRAMEWORK_MODEL_H

#ifdef TINYOBJLOADER
#include "Shapes.h"
#include "DebugModel.h"
class Material;

class LoadedModel;

class Model {
public:
    GLuint vao = 0;
    GLsizei indexCount;
    GLenum indexType;

    int transformID = 0;
    Material *material = nullptr;
    const LoadedModel *loadedModel = nullptr;
    Shapes::BoundingSphere bounds = {{0, 0, 0}, 9999.0f};
    Shapes::AABB boundingBox;

    void Draw() const;
    int TriangleCount() const;
    std::string name;

    DebugModel vertexNormal;
    DebugModel faceNormal;
};
#elif ASSIMPLOADER
#include "Mesh.h"
#include "Material.h"
class Model {
 public:
  explicit Model(char *path) { loadModel(path); }
  void Draw() const;
  Material *material = nullptr;
  int TriangleCount() const;
  int transformID = 0;
 private:
  // model data
  std::vector<Mesh> meshes;
  std::string directory;

  void loadModel(std::string path);
  void processNode(aiNode *node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);
  std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                       std::string typeName);
};
#endif

#endif //OPENGLFRAMEWORK_MODEL_H
