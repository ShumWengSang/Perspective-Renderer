//
// Created by user on 1/30/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: Math.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/30/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "MyMath.h"
#include "Model.h"
#include "ModelSystem.h"
#include "TransformSystem.h"


glm::vec4 MyMath::Remap(const glm::vec4 &value, const glm::vec2 &inMinMax, const glm::vec2 &outMinMax) {
    return outMinMax.x + (value - inMinMax.x) * (outMinMax.y - outMinMax.x) / (inMinMax.y - inMinMax.x);
}

float MyMath::Remap(float value, float low1, float high1, float low2, float high2) {
    return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

void MyMath::CsoSupport(
        const Model &modelA, const Model &modelB, const glm::vec3 &dir, glm::vec3 &support, glm::vec3 &supportA
        , glm::vec3 &supportB
                       ) {
    Transform &transformA = TransformSystem::getInstance().Get(modelA.transformID);
    Transform &transformB = TransformSystem::getInstance().Get(modelA.transformID);

    // Convert search direction to model space
    const glm::vec3 localDirA = transformA.WorldToLocal(glm::vec4(dir, 0));
    const glm::vec3 localDirB = transformA.WorldToLocal(glm::vec4(-dir, 0));

    // Compute support points in model space

}

glm::vec3 MyMath::FindSupportPoint(const std::vector<Vertex> &vertices, const glm::vec3 &dir) {
    float highest = std::numeric_limits<float>::max();
    glm::vec3 support{0, 0, 0};
    for (int i = 0; i < vertices.size(); i++) {
        glm::vec3 v = vertices[i].position;
        float dot = glm::dot(dir, v);
        if (dot > highest) {
            highest = dot;
            support = v;
        }
    }
    return support;
}

MyMath::Quaternion MyMath::Slerp(const MyMath::Quaternion& begin, const MyMath::Quaternion& end, float factor)
{
    float cosTheta = begin.Dot(end);
    MyMath::Quaternion localEnd = end;
    if (cosTheta < 0)
    {
        localEnd = (end * -1);
        cosTheta = -cosTheta;
    }

    // Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
    if (cosTheta > 1.0 - 0.0004)
    {
        // Linear interpolation all components
        return MyMath::Quaternion(
            MyMath::Lerp(begin.s, end.s, factor),
            MyMath::Lerp(begin.v, end.v, factor));
    }
    
    float angle = acos(cosTheta);

    return (begin * sin((1.0f - factor) * angle) + end * sin(factor * angle)) / sin(angle);
}

glm::vec3 MyMath::Slerp(const glm::vec3& begin, const glm::vec3& end, float factor)
{
    float cosTheta = glm::dot(begin, end);
    glm::vec3 localEnd = end;
    if (cosTheta < 0)
    {
        localEnd = (end * -1);
        cosTheta = -cosTheta;
    }

    float angle = acos(cosTheta);
    return (begin * (sin((1 - factor) * angle)) + localEnd * sin(factor * angle)) * (1 / sin(angle));
}

glm::vec3 MyMath::Lerp(const glm::vec3& begin, const glm::vec3& end, float factor)
{
    return (1 - factor) * begin + factor * end;
}
float MyMath::Lerp(float begin, float end, float factor)
{
    return (1 - factor) * begin + factor * end;
}
#if TINYOBJLOADER
glm::vec3 MyMath::FindSupportPoint(const std::vector<Shapes::Triangle> &trigs, const glm::vec3 &dir) {
    float highest = std::numeric_limits<float>::max();
    glm::vec3 support{0, 0, 0};
    for (int i = 0; i < trigs.size(); i++) {
        glm::vec3 v[] = {trigs[i].v1, trigs[i].v2, trigs[i].v3};
        for (int i = 0; i < 3; i++) {
            float dot = glm::dot(dir, v[i]);
            if (dot > highest) {
                highest = dot;
                support = v[i];
            }
        }
    }
    return support;
}

#endif