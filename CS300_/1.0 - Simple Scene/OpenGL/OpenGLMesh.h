//
// Created by roland on 9/30/20.
//

#ifndef SIMPLE_SCENE_OPENGLMESH_H
#define SIMPLE_SCENE_OPENGLMESH_H

#include "../ObjLoader.h"

class OpenGLMesh : public Mesh
{
public:
    Mesh &SetVerticeBuffer(SharedPtr<VertexArray> &vaoOwner) override;

    Mesh &GenerateRandomColors(SharedPtr<VertexArray> &vaoOwner) override;

    Mesh &SetIndexBuffer(SharedPtr<VertexArray> &vaoOwner) override;

    Mesh& CalcVertexNormal(SharedPtr<VertexArray>& vaoOwner) override;

    Mesh &GenerateVertexNormalLines(SharedPtr<VertexArray> &vaoOwner) override;

    Mesh &SetVertexNormal(SharedPtr<VertexArray> &vaoOwner) override;

    Mesh &GenerateFaceNormalLines(SharedPtr<VertexArray> &vaoOwner) override;

    Mesh &GenerateVertexColor(SharedPtr<VertexArray> &vaoOwner) override;
};


#endif //SIMPLE_SCENE_OPENGLMESH_H
