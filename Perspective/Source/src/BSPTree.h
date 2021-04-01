//
// Created by user on 3/31/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: BSPTree.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Perspective
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 3/31/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_BSPTREE_H
#define OPENGLFRAMEWORK_BSPTREE_H
#include "Shapes.h"

struct BSPRenderSettings
{
    bool renderNodes = true;
    bool renderPlanes = false;
};

class CollisionMesh;
struct BSPNode {
    BSPNode(BSPNode* pVoid, rapidjson::Value& value);

    BSPNode* Parent = nullptr;
    Shapes::Plane splitPlane = Shapes::Plane(glm::vec4(0,0,0,0));
    glm::vec3 color;
    BSPNode * left = nullptr;
    BSPNode * right = nullptr;
    std::vector<Shapes::Triangle> Objects;
    BSPNode(BSPNode* parent, std::vector<Shapes::Triangle> const & vertices, int maxNumTrigs,
            int depth, bool &depthTerminated);
    ~BSPNode();
    void RenderNode(BSPRenderSettings const & settings);
    bool isLeaf() const;
    void Clear();
    static int MAX_DEPTH;
    static int Good_Enough_Score;
    static int Plane_Sample_Size;
    static float k_value;
private:
    Shapes::Plane PickSplittingPlaneFromPolygon(std::vector<Shapes::Triangle> const & trigs, float k, int depth);
    Shapes::Plane PickSplittingPlane(std::vector<Shapes::Triangle> const & trigs, float k, int depth);
    Shapes::Plane PickSplittingPlaneFromRandom(std::vector<Shapes::Triangle> const & trigs);
    const glm::vec3& ChooseRandomPoint(std::vector<Shapes::Triangle> const &  trigs);
    const Shapes::Triangle& ChooseRandomTrig(std::vector<Shapes::Triangle> const &  trigs);

public:
    template <typename Writer>
    inline void Serialize(Writer& writer) const {
        writer.StartObject();

        writer.String(("Color"));
        SerializeVec3(writer, this->color);

        writer.String("Plane");
        this->splitPlane.Serialize(writer);

        if(left) {
            writer.String(("Left"));
            left->Serialize(writer);
        }
        if(right) {
            writer.String(("Right"));
            right->Serialize(writer);
        }

        writer.String(("Objects"));
        writer.StartArray();
        for (auto dependentItr = Objects.begin(); dependentItr != Objects.end(); ++dependentItr)
            dependentItr->Serialize(writer);
        writer.EndArray();

        writer.EndObject();
    }
};

class BSPTree {
public:
    BSPTree(int sizeEachNode, std::vector<CollisionMesh> const & collisionMeshes);
    void ImGuiSettings();
    void ForwardRenderTree() const;
    ~BSPTree();
private:
    BSPNode* root = nullptr;
    BSPRenderSettings settings;

public:
    explicit BSPTree(rapidjson::Document & doc);
    template <typename Writer>
    inline void Serialize(Writer& writer) const {
        writer.StartObject();

        writer.String("Root");
        root->Serialize(writer);

        writer.EndObject();
    }
};


#endif //OPENGLFRAMEWORK_BSPTREE_H
