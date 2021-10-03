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


#endif //OPENGLFRAMEWORK_MODEL_H
