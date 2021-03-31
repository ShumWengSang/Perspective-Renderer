//
// Created by user on 3/21/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: OctTree.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Perspective
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 3/21/2021
 * End Header --------------------------------------------------------*/
#include <glm/ext.hpp>
#include "stdafx.h"
#include "OctTree.h"
#include "Collision.h"
#include "Logger.h"
#include "Model.h"
#include "ModelSystem.h"

int OctTreeNode::MAX_DEPTH = 100;

OctTree::OctTree(int sizeEachNode, const std::vector<CollisionMesh> &collisionMeshes) {
    AllTriangle.reserve(1024 * 4);

    // Get the total of all collision mesh

    glm::vec3 min {INFINITY};
    glm::vec3 max {-INFINITY};
    for(int i = 0; i < collisionMeshes.size(); ++i)
    {
        AllTriangle.insert(AllTriangle.end(), collisionMeshes[i].vertices.begin(), collisionMeshes[i].vertices.end());
    }

    // Get min max for Octtree
    for(int i = 0; i < AllTriangle.size(); ++i)
    {
        min.x = fmin(min.x, AllTriangle[i].v1.x);
        min.y = fmin(min.y, AllTriangle[i].v1.y);
        min.z = fmin(min.z, AllTriangle[i].v1.z);
        min.x = fmin(min.x, AllTriangle[i].v2.x);
        min.y = fmin(min.y, AllTriangle[i].v2.y);
        min.z = fmin(min.z, AllTriangle[i].v2.z);
        min.x = fmin(min.x, AllTriangle[i].v3.x);
        min.y = fmin(min.y, AllTriangle[i].v3.y);
        min.z = fmin(min.z, AllTriangle[i].v3.z);

        max.x = fmax(max.x, AllTriangle[i].v1.x);
        max.y = fmax(max.y, AllTriangle[i].v1.y);
        max.z = fmax(max.z, AllTriangle[i].v1.z);
        max.x = fmax(max.x, AllTriangle[i].v2.x);
        max.y = fmax(max.y, AllTriangle[i].v2.y);
        max.z = fmax(max.z, AllTriangle[i].v2.z);
        max.x = fmax(max.x, AllTriangle[i].v3.x);
        max.y = fmax(max.y, AllTriangle[i].v3.y);
        max.z = fmax(max.z, AllTriangle[i].v3.z);
    }
    min *= 1.1f;
    max *= 1.1f;
    Shapes::AABB boundingVolume(min, max, 0);
    Log("Bounding box Min [%f,%f,%f] and Max [%f,%f,%f]\n", min.x, min.y, min.z, max.x, max.y, max.z);

    bool depthTerminate = false;
    // Generate Oct Tree
    this->root = new OctTreeNode(nullptr, AllTriangle, sizeEachNode, boundingVolume, 0, depthTerminate);

    if(depthTerminate)
    {
        Log("Oct Tree creation terminated due to max depth!");
    }
    else
    {
        Log("Oct Tree has finished creation!");
    }
}

OctTree::~OctTree() {
    if(root)
        root->Clear();
    delete root;
}

void OctTree::ForwardRenderTree() const {
    static glm::vec3 green = glm::vec3(0,1,0);
    if(settings.renderRootPoints) {
        for (int i = 0; i < AllTriangle.size(); ++i) {
            dd::point(glm::value_ptr(AllTriangle[i].v1), glm::value_ptr(green), 5);
            dd::point(glm::value_ptr(AllTriangle[i].v2), glm::value_ptr(green), 5);
            dd::point(glm::value_ptr(AllTriangle[i].v3), glm::value_ptr(green), 5);
        }
    }

    if(root)
        root->RenderNode(settings);
}

void OctTree::ImGuiSettings() {
    if(ImGui::TreeNode("Oct Tree Rendering Settings")) {
        ImGui::Checkbox("renderAABB", &settings.renderAABB);
        ImGui::Checkbox("renderLines", &settings.renderLines);
        ImGui::Checkbox("renderRootPoints", &settings.renderRootPoints);
        ImGui::Checkbox("renderOnlyLeaf", &settings.renderOnlyLeaf);
        ImGui::Checkbox("renderLeafPoints", &settings.renderLeafPoints);
        ImGui::TreePop();
        ImGui::Separator();
    }
}

OctTreeNode::OctTreeNode(
        OctTreeNode *parent, const std::vector<Shapes::Triangle> &triangles, int maxNumTrigs
        , const Shapes::AABB &boundingVolume, int depth,  bool &depthTerminated
                        ) :
        Parent(parent), boundingVolume(boundingVolume)
{
    if(depth >= OctTreeNode::MAX_DEPTH)
    {
        depthTerminated = true;
        return;
    }

    std::vector<Shapes::Triangle> tempInBoundsTriangles;
    tempInBoundsTriangles.reserve(maxNumTrigs);
    // Check if each triangle is within our bounds
    for(int i = 0; i < triangles.size(); ++i)
    {
        // Send them into a collision function to check against our bounds, return a std::vector<Triangle>
        std::vector<Shapes::Triangle> result = triangles[i].BoundingVolumeCut(boundingVolume);

        if(!result.empty())
        {
            // Put result into temp array
            tempInBoundsTriangles.insert(tempInBoundsTriangles.end(), result.begin(), result.end());
            this->numberInBox += result.size();
        }
        else
        {
            this->numberOutBox++;
        }
    }
    numberGenerated = std::max<int>(0,(numberInBox + numberOutBox) - triangles.size());
    // Log("%*s This node has %d in box and %d out of the box. Generated %d\n", depth, "", numberInBox, numberOutBox, numberGenerated);

    // If there are too many triangles in our node
    if(numberInBox > maxNumTrigs)
    {
        // We split them up
        std::vector<Shapes::AABB> tempBounds = CreateBoundingVolumes(boundingVolume);
        Nodes.resize(8, nullptr);

        for(int i = 0; i < 8; ++i) {
            Nodes[i] = new OctTreeNode(this, tempInBoundsTriangles, maxNumTrigs, tempBounds[i], depth + 1, depthTerminated);
            // If nothing inside node
            if(Nodes[i]->numberInBox == 0)
            {
                // Delete node
                delete Nodes[i];
                Nodes[i] = nullptr;
            }
        }
    }
    else // Then we are a leaf node.
    {
        // Change color too
        color = glm::vec4(glm::linearRand(glm::vec3(0,0,0), glm::vec3(1,1,1)), 1);
        Objects = std::move(tempInBoundsTriangles);
    }
}

bool OctTreeNode::isLeaf() const {
    return Objects.size() != 0;
}

void OctTreeNode::Clear() {
    for(int i = 0; i < Nodes.size(); ++i)
    {
        if(Nodes[i])
            Nodes[i]->Clear();
        delete Nodes[i];
        Nodes[i] = nullptr;
    }
}

std::vector<Shapes::AABB> OctTreeNode::CreateBoundingVolumes(const Shapes::AABB &parentBoundingVolume) const {
    (void)parentBoundingVolume;
    auto [Min, Max] = boundingVolume.GetMinMax();
    glm::vec3 maxPointHalf = (Max - Min) * 0.5f;
    std::vector<Shapes::AABB> res(8, Shapes::AABB());

    res[0] = Shapes::AABB(glm::vec3(Min.x, Min.y, Min.z),
                          glm::vec3(Max.x, Max.y, Max.z) - maxPointHalf, 0);

    res[1] = Shapes::AABB(glm::vec3(Min.x + maxPointHalf.x, Min.y, Min.z),
                          glm::vec3(Max.x + maxPointHalf.x, Max.y, Max.z) - maxPointHalf, 0);

    res[2] = Shapes::AABB(glm::vec3(Min.x, Min.y + maxPointHalf.y, Min.z),
                          glm::vec3(Max.x, Max.y + maxPointHalf.y, Max.z) - maxPointHalf, 0);

    res[3] = Shapes::AABB(glm::vec3(Min.x + maxPointHalf.x, Min.y + maxPointHalf.y, Min.z),
                          glm::vec3(Max.x+ maxPointHalf.x, Max.y + maxPointHalf.y, Max.z) - maxPointHalf, 0);

    res[4] = Shapes::AABB(glm::vec3(Min.x, Min.y, Min.z + maxPointHalf.z),
                          glm::vec3(Max.x, Max.y, Max.z + maxPointHalf.z) - maxPointHalf, 0);

    res[5] = Shapes::AABB(glm::vec3(Min.x + maxPointHalf.x, Min.y, Min.z + maxPointHalf.z),
                          glm::vec3(Max.x + maxPointHalf.x, Max.y, Max.z + maxPointHalf.z) - maxPointHalf, 0);

    res[6] = Shapes::AABB(glm::vec3(Min.x, Min.y + maxPointHalf.y, Min.z + maxPointHalf.z),
                          glm::vec3(Max.x, Max.y + maxPointHalf.y, Max.z + maxPointHalf.z) - maxPointHalf, 0);

    res[7] = Shapes::AABB(glm::vec3(Min.x + maxPointHalf.x, Min.y + maxPointHalf.y, Min.z + + maxPointHalf.z),
                          glm::vec3(Max.x + maxPointHalf.x, Max.y + maxPointHalf.y, Max.z + maxPointHalf.z) - maxPointHalf, 0);

    return res;
}

void OctTreeNode::RenderNode(OctTreeRenderSettings const & settings) const {

    if(settings.renderAABB) {
        if(settings.renderOnlyLeaf) {
            if (this->isLeaf()) {
                boundingVolume.RenderAABB(this->color);
            }
        }
        else {
            boundingVolume.RenderAABB(this->color);
        }
    }

    for(int i = 0; i < this->Nodes.size(); i++)
    {
        if(Nodes[i])
            Nodes[i]->RenderNode(settings);
    }

    if(settings.renderOnlyLeaf) {
        if (this->isLeaf()) {
            if (settings.renderLines) {

                for (int i = 0; i < this->Objects.size(); ++i) {
                    glm::vec3 red = glm::vec3(1, 0, 0);

                    dd::line(glm::value_ptr(Objects[i].v1), glm::value_ptr(Objects[i].v2), glm::value_ptr(this->color));
                    dd::line(glm::value_ptr(Objects[i].v2), glm::value_ptr(Objects[i].v3), glm::value_ptr(this->color));
                    dd::line(glm::value_ptr(Objects[i].v3), glm::value_ptr(Objects[i].v1), glm::value_ptr(this->color));
                }
            }
            if (settings.renderLeafPoints) {
                for (int i = 0; i < this->Objects.size(); ++i) {
                    dd::point(glm::value_ptr(Objects[i].v1), glm::value_ptr(this->color), 5);
                    dd::point(glm::value_ptr(Objects[i].v2), glm::value_ptr(this->color), 5);
                    dd::point(glm::value_ptr(Objects[i].v3), glm::value_ptr(this->color), 5);
                }
            }
        }
    }
    else
    {
        if (settings.renderLines) {

            for (int i = 0; i < this->Objects.size(); ++i) {
                glm::vec3 red = glm::vec3(1, 0, 0);

                dd::line(glm::value_ptr(Objects[i].v1), glm::value_ptr(Objects[i].v2), glm::value_ptr(this->color));
                dd::line(glm::value_ptr(Objects[i].v2), glm::value_ptr(Objects[i].v3), glm::value_ptr(this->color));
                dd::line(glm::value_ptr(Objects[i].v3), glm::value_ptr(Objects[i].v1), glm::value_ptr(this->color));
            }
        }
    }
}

void OctTreeNode::GetBoundingBoxes(std::vector<Shapes::AABB> &output) {
    // If we are leaf, we add our box in, else we continue in
    if(this->isLeaf())
    {
        output.emplace_back(this->boundingVolume);
    }
    else
    {
        for(auto child : Nodes)
        {
            if(child)
            {
                GetBoundingBoxes(output);
            }
        }
    }
}
