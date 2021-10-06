//
// Created by user on 1/30/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: Math.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/30/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_MYMATH_H
#define OPENGLFRAMEWORK_MYMATH_H
#include "Quaternions.h"
class Model;

struct Vertex;
namespace Shapes {
    class Triangle;
}
namespace MyMath {

    float Remap(float value, float low1, float high1, float low2, float high2);

    glm::vec4 Remap(const glm::vec4 &value, const glm::vec2 &inMinMax, const glm::vec2 &outMinMax);

    void CsoSupport(
            const Model &modelA, const Model &modelB, const glm::vec3 &dir, glm::vec3 &support, glm::vec3 &supportA
            , glm::vec3 &supportB
                   );

    glm::vec3 FindSupportPoint(std::vector<Shapes::Triangle> const &trigs, glm::vec3 const &dir);

    glm::vec3 FindSupportPoint(std::vector<Vertex> const &vertices, glm::vec3 const &dir);

    inline glm::mat4 AssimpToMat4(const aiMatrix4x4& assimpMat)
    {
        return glm::transpose(glm::make_mat4(&assimpMat.a1)); // Convert to normal glm::mat4
    }


    inline glm::vec3 AssimpToGLMVec3(const aiVector3D& v) { return glm::vec3(v.x, v.y, v.z); }

    MyMath::Quaternion Slerp(const MyMath::Quaternion& begin, const MyMath::Quaternion& end, float factor);
    glm::vec3 Slerp(const glm::vec3& begin, const glm::vec3& end, float factor);

    glm::vec3 Lerp(const glm::vec3& begin, const glm::vec3& end, float factor);
    float Lerp(float begin, float end, float factor);
    struct iSlerp {
        int a;
    };

    struct iVQS {
        int a;
    };
};


#endif //OPENGLFRAMEWORK_MYMATH_H
