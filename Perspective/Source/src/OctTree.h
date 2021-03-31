//
// Created by user on 3/21/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: OctTree.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Perspective
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 3/21/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_OCTTREE_H
#define OPENGLFRAMEWORK_OCTTREE_H

#include "Shapes.h"

class CollisionMesh;
class Model;

struct OctTreeRenderSettings
{
    bool renderAABB = true;
    bool renderLines = false;
    bool renderRootPoints = false;
    bool renderOnlyLeaf = false;
    bool renderLeafPoints = true;
};

struct OctTreeNode
{
    std::vector<Shapes::Triangle> Objects;
    std::vector<OctTreeNode*> Nodes;
    OctTreeNode* Parent = nullptr;
    glm::vec4 color;
    Shapes::AABB boundingVolume;

    // Debug
    int numberInBox = 0;
    int numberOutBox = 0;
    int numberGenerated = 0;

    OctTreeNode(OctTreeNode* parent, std::vector<Shapes::Triangle> const & vertices, int maxNumTrigs,
                Shapes::AABB const & boundingVolume, int depth, bool &depthTerminated);

    bool isLeaf() const;

    void Clear();

    std::vector<Shapes::AABB> CreateBoundingVolumes(const Shapes::AABB& parentBoundingVolume) const;

    void RenderNode(OctTreeRenderSettings const & settings) const;
    void GetBoundingBoxes(std::vector<Shapes::AABB>& output);

    static int MAX_DEPTH;

    template <typename Writer>
    inline void Serialize(Writer& writer) const {
        writer.StartObject();

        writer.String(("Color"));
        SerializeVec3(writer, glm::vec3(this->color));


        writer.String(("Bounding Volume"));
        boundingVolume.Serialize(writer);

        writer.String(("Objects"));
        writer.StartArray();
        for (auto dependentItr = Objects.begin(); dependentItr != Objects.end(); ++dependentItr)
            dependentItr->Serialize(writer);
        writer.EndArray();


        writer.String(("Child Nodes"));
        writer.StartArray();
        for (auto dependentItr = Nodes.begin(); dependentItr != Nodes.end(); ++dependentItr) {
            if(*dependentItr)
                (*dependentItr)->Serialize(writer);
        }
        writer.EndArray();

        writer.EndObject();
    }
};

class OctTree {
public:
    OctTree(int sizeEachNode, std::vector<CollisionMesh> const & collisionMeshes);
    ~OctTree();
    void ForwardRenderTree() const;
    void ImGuiSettings();
private:
    std::vector<Shapes::Triangle> AllTriangle;
    OctTreeNode *root;
    OctTreeRenderSettings settings;
public:
    template <typename Writer>
    inline void Serialize(Writer& writer) const {
        writer.StartObject();

/*        writer.String(("AllTriangle"));
        writer.StartArray();
        for (auto dependentItr = AllTriangle.begin(); dependentItr != AllTriangle.end(); ++dependentItr)
            dependentItr->Serialize(writer);
        writer.EndArray();*/

        writer.String("Root");
        root->Serialize(writer);

        writer.EndObject();
    }
};


#endif //OPENGLFRAMEWORK_OCTTREE_H
