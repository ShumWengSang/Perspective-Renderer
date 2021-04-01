//
// Created by user on 3/31/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: BSPTree.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Perspective
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 3/31/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "BSPTree.h"
#include "Logger.h"
#include "Collision.h"


int BSPNode::MAX_DEPTH = 100;
int BSPNode::Good_Enough_Score = 100;
int BSPNode::Plane_Sample_Size = 10;
float BSPNode::k_value = 0.8f;

BSPTree::BSPTree(int sizeEachNode, const std::vector<CollisionMesh> &collisionMeshes) {
    std::vector<Shapes::Triangle> AllTriangle;
    AllTriangle.reserve(1024 * 4);

    for(int i = 0; i < collisionMeshes.size(); ++i)
    {
        AllTriangle.insert(AllTriangle.end(), collisionMeshes[i].vertices.cbegin(), collisionMeshes[i].vertices.cend());
    }

    bool depthTerminate = false;
    this->root = new BSPNode(nullptr, AllTriangle, sizeEachNode, 0, depthTerminate);

    if(depthTerminate)
    {
        Log("BSP Tree creation terminated due to max depth!");
    }
    else
    {
        Log("BSP Tree has finished creation!");
    }
}

void BSPTree::ImGuiSettings() {
    if(ImGui::TreeNode("BST Tree Rendering Settings")) {
        ImGui::Checkbox("Render Nodes", &settings.renderNodes);
        ImGui::Checkbox("Render Planes", &settings.renderPlanes);
        ImGui::TreePop();
        ImGui::Separator();
    }

}

void BSPTree::ForwardRenderTree() const {
    root->RenderNode(settings);
}

BSPTree::~BSPTree() {
    delete root;
}

BSPTree::BSPTree(rapidjson::Document &doc) {
    using namespace rapidjson;
    Value& value = doc["Root"];
    root = new BSPNode(nullptr, value);
}

BSPNode::BSPNode(
        BSPNode *parent, const std::vector<Shapes::Triangle> &trigs, int maxNumTrigs, int depth
        , bool &depthTerminated
                ) : Parent(parent) {
    if(depth >= MAX_DEPTH)
    {
//        this->Objects = trigs;
        depthTerminated = true;
//        color = glm::linearRand(glm::vec3(0,0,0), glm::vec3(1,1,1));
        return;
    }




    // We need to split
    if(trigs.size() > maxNumTrigs)
    {
        // Find the best splitting plane
        this->splitPlane = PickSplittingPlane(trigs, k_value, depth);

        static std::vector<Shapes::Point3D> polygon;
        static std::vector<Shapes::Point3D> frontPlane;
        static std::vector<Shapes::Point3D> backPlane;
        polygon.reserve(3);
        std::vector<Shapes::Triangle> leftTrigs;
        leftTrigs.reserve(trigs.size() / 2);
        std::vector<Shapes::Triangle> rightTrigs;
        rightTrigs.reserve(trigs.size() / 2);

        polygon.clear();
        frontPlane.clear();
        backPlane.clear();

        // Split each vertices by plane
        for(int i = 0; i < trigs.size(); ++i)
        {
            polygon.emplace_back(trigs[i].v1);
            polygon.emplace_back(trigs[i].v2);
            polygon.emplace_back(trigs[i].v3);
            switch (Shapes::ClassifyPolyonToPlane(polygon, splitPlane))
            {
                case Shapes::POLYGON_COPLANAR_WITH_PLANE:
                    //Fall through since we want leaf nodes
                case Shapes::POLYGON_IN_FRONT_OF_PLANE:
                    leftTrigs.emplace_back(trigs[i]);
                    break;
                case Shapes::POLYGON_BEHIND_PLANE:
                    rightTrigs.emplace_back(trigs[i]);
                    break;
                case Shapes::POLYGON_STRADDLING_PLANE:
                {

                    // Split polygone to plane and send a part to each side
                    SuthHodgeClip(polygon, splitPlane, backPlane, frontPlane);
                    auto frontPlaneTrigs = Shapes::ReassemblePointsToTriangle(frontPlane);
                    leftTrigs.insert(leftTrigs.end(), frontPlaneTrigs.begin(), frontPlaneTrigs.end());

                    auto backPlaneTrigs = Shapes::ReassemblePointsToTriangle(backPlane);
                    rightTrigs.insert(rightTrigs.end(), backPlaneTrigs.begin(), backPlaneTrigs.end());
                    break;
                }
            }

            polygon.clear();
            backPlane.clear();
            frontPlane.clear();
        }

        // Split successful.
        left = new BSPNode(this, leftTrigs, maxNumTrigs, depth + 1, depthTerminated);
        right = new BSPNode(this, rightTrigs, maxNumTrigs, depth + 1, depthTerminated);
    }
    else
    {
        // No need to split
        this->Objects = trigs;
        // Generate random color
        color = glm::linearRand(glm::vec3(0,0,0), glm::vec3(1,1,1));
    }

}

BSPNode::~BSPNode() {
    Clear();
}

bool BSPNode::isLeaf() const {
    return !Objects.empty();
}

void BSPNode::Clear() {
    delete left;
    delete right;
}

void BSPNode::RenderNode(BSPRenderSettings const & settings) {
    if(this->isLeaf()) {
        if (settings.renderNodes) {
            for (int i = 0; i < this->Objects.size(); ++i) {
                dd::line(glm::value_ptr(Objects[i].v1), glm::value_ptr(Objects[i].v2), glm::value_ptr(this->color));
                dd::line(glm::value_ptr(Objects[i].v2), glm::value_ptr(Objects[i].v3), glm::value_ptr(this->color));
                dd::line(glm::value_ptr(Objects[i].v3), glm::value_ptr(Objects[i].v1), glm::value_ptr(this->color));
            }
        }
    }
    else
    {
        if(settings.renderPlanes)
        {
            glm::vec3 point (1,1,0);
            point.z = this->splitPlane.Normal.w - this->splitPlane.Normal.x * point.x - this->splitPlane.Normal.y - point.y;
            glm::vec3 red = {1.0f,0.f,0.f};
            glm::vec3 green = {1.0f,1.f,0.f};
            dd::plane(glm::value_ptr(point), glm::value_ptr(this->splitPlane.Normal),
                      glm::value_ptr(red), glm::value_ptr(green), 3, 1);
        }
        if(left)
            left->RenderNode(settings);
        if(right)
            right->RenderNode(settings);
    }
}

Shapes::Plane BSPNode::PickSplittingPlaneFromPolygon(const std::vector<Shapes::Triangle> &trigs, float k, int depth) {
    // From the Orange Book

    // Blend factor for optimizing for balance or splits (should be tweaked)
    const float K = k;

    // Variables for tracking best splitting plane seen so far
    Shapes::Plane bestPlane (glm::vec4(0,0,0,0));
    float bestScore = FLT_MAX;

    std::vector<Shapes::Point3D> polygon;
    polygon.reserve(3);

    // We need to determine if we want to randomize splits or not.
    // Do this based on depth. Start with a random split
    bool randomSplit = true;
    if(depth % 2 == 0)
        randomSplit = false;

    // We will run it 10 times or until we get a score that good enough for us
    for(int j = 0; j < Plane_Sample_Size && bestScore > Good_Enough_Score; ++j)
    {
        Shapes::Plane tempPlane = Shapes::Plane(glm::vec3(), 0);
        int numInFront = 0, numBehind = 0, numStraddling = 0;

        if(randomSplit)
        {
            tempPlane = PickSplittingPlaneFromRandom(trigs);
        }
        else
        {
            // Pick a random polygon, then get the plane for that.
            tempPlane = ChooseRandomTrig(trigs).GetPlane();
        }

        // Keep standing count of various poly-plane relationships
        polygon.emplace_back(trigs[j].v1);
        polygon.emplace_back(trigs[j].v2);
        polygon.emplace_back(trigs[j].v3);
        switch(Shapes::ClassifyPolyonToPlane(polygon, tempPlane)){
            case Shapes::POLYGON_COPLANAR_WITH_PLANE:
                /* Coplanar polygons treated as being in front of plane */
            case Shapes::POLYGON_IN_FRONT_OF_PLANE:
                numInFront++;
                break;
            case Shapes::POLYGON_BEHIND_PLANE:
                numBehind++;
                break;
            case Shapes::POLYGON_STRADDLING_PLANE:
                numStraddling++;
                break;
        }
        polygon.clear();
        // Compute score as a weighted combination
        float score = K * numStraddling + (1.0f - K) * std::abs(numInFront - numBehind);
        if(score < bestScore){
            bestScore = score;
            bestPlane = tempPlane;
        }
    }

    return bestPlane;
}
const glm::vec3& BSPNode::ChooseRandomPoint(std::vector<Shapes::Triangle> const & trigs)
{
    const Shapes::Triangle& randomTrig = trigs[glm::linearRand<int>(0, trigs.size() - 1)];
    return randomTrig.v1;
}

Shapes::Plane BSPNode::PickSplittingPlane(const std::vector<Shapes::Triangle> &trigs, float k, int depth) {

    // First three depth, use X axis, Y axis, and Z axis
/*    if(depth <= 2)
    {
        glm::vec3 normals [3] = {glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec3(0,0,1)};
        auto randomPoint = ChooseRandomPoint(trigs);
        float d = glm::dot(normals[depth], randomPoint);
        return Shapes::Plane(normals[depth], d);
    }*/
    // If the number of trigs is above x count, we will use random split
    return PickSplittingPlaneFromPolygon(trigs, k, depth);
}

Shapes::Plane BSPNode::PickSplittingPlaneFromRandom(const std::vector<Shapes::Triangle> &trigs) {
        // Randomize a normal
        // Then randomize a triangle point to use
        const float val = 100.f;
        glm::vec3 normal = glm::linearRand(glm::vec3(-val,-val,-val), glm::vec3(val,val,val));

        const auto& randomPoint = ChooseRandomPoint(trigs);
        normal = glm::normalize(normal);
        float d = glm::dot(normal, randomPoint);
        return Shapes::Plane(normal, d);
}

const Shapes::Triangle &BSPNode::ChooseRandomTrig(const std::vector<Shapes::Triangle> &trigs) {
    return trigs[glm::linearRand<int>(0, trigs.size() - 1)];;
}

BSPNode::BSPNode(BSPNode* pVoid, rapidjson::Value& value) : Parent(pVoid) {
    splitPlane = Shapes::Plane::Deserialize(value["Plane"]);
    color = DeserializeVec3(value["Color"]);
    if(value.HasMember("Left"))
    {
        left = new BSPNode(this, value["Left"]);
    }
    if(value.HasMember("Right"))
    {
        right = new BSPNode(this, value["Right"]);
    }
    for(auto objectsIterator = value["Objects"].Begin(); objectsIterator != value["Objects"].End(); ++objectsIterator)
    {
        Objects.emplace_back(Shapes::Triangle::Deserialize(*objectsIterator));
    }
}
