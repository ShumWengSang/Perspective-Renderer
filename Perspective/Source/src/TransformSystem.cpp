//
// Created by user on 1/24/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: TransformSystem.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/24/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "TransformSystem.h"

// Internal
static bool IdenticalTransformProperties(const Transform &a, const Transform &b) {
    return a.position == b.position
           && a.orientation == b.orientation
           && a.scale == b.scale;
}

Transform& Transform::SetLocalPosition(glm::vec3 pos) {
    position = pos;
    return *this;
}

Transform &Transform::SetLocalPosition(float x, float y, float z) {
    position.x = x;
    position.y = y;
    position.z = z;
    return *this;
}

Transform &Transform::SetLocalScale(float s) {
    SetLocalScale(s, s, s);
    return *this;
}

Transform &Transform::SetLocalScale(float x, float y, float z) {
    scale.x = x;
    scale.y = y;
    scale.z = z;
    return *this;
}


Transform &Transform::SetLocalDirection(float x, float y, float z) {

    // Switch x and z if 90� off!
    orientation = glm::vec3(x, y, z);
    return *this;
}

glm::vec4 Transform::LocalToWorld(const glm::vec4 &vec) const {
    return this->matrix * vec;
}

glm::vec4 Transform::WorldToLocal(const glm::vec4 &vec) const {
    return this->inverseMatrix * vec;
}


void TransformSystem::Init() {

}

void TransformSystem::Update() {
    size_t numTransforms = nextIndex;
    for (size_t id = 0; id < numTransforms; ++id) {
        Transform &old = oldTransforms[id];
        Transform &curr = transforms[id];

        if (!IdenticalTransformProperties(old, curr)) {
            old = curr;
        }
    }
}

int TransformSystem::Create() {
    assert(nextIndex < MAX_NUM_TRANSFORMS);
    int transformID = nextIndex++;
    return transformID;
}

Transform &TransformSystem::Get(int transformID) {
    Transform &transform = transforms[transformID];
    return transform;
}

const Transform &TransformSystem::GetPrevious(int transformID) {
    const Transform &transform = oldTransforms[transformID];
    return transform;
}

void TransformSystem::UpdateMatrices(int transformID) {
    const Transform &old = oldTransforms[transformID];
    Transform &curr = transforms[transformID];

    // Unless nothing has changed, create new matrices for the transform
    if (!IdenticalTransformProperties(old, curr)) { 
        curr.matrix = glm::translate(glm::mat4(1.0f), curr.position);
        curr.matrix = curr.matrix *
            glm::yawPitchRoll(
                glm::radians(curr.orientation.y),
                glm::radians(curr.orientation.x),
                glm::radians(curr.orientation.z));
        curr.matrix = glm::scale(curr.matrix, curr.scale);

        curr.inverseMatrix = glm::inverse(curr.matrix);
        curr.normalMatrix = glm::transpose(glm::inverse(glm::mat3{curr.matrix}));
    }
}

void TransformSystem::Destroy() {

}
