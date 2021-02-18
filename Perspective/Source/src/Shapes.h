//
// Created by user on 1/25/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: Shapes.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/25/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_SHAPES_H
#define OPENGLFRAMEWORK_SHAPES_H

namespace Shapes
{
    struct BoundingSphere
    {
        glm::vec3 center;
        float radius;
    };

    glm::vec3 SrgbColor(float r, float g, float b);

    void ExtractFrustumPlanes(const glm::mat4& matrix, std::array<glm::vec4, 6>& planes);
    void NormalizePlane(glm::vec4& plane);

    float VectorMaxComponent(const glm::vec3& vector);

    bool InPositiveHalfSpace(const glm::vec4& plane, const BoundingSphere& boundingSphere, float epsilon = 0.01f);
    bool InsideFrustum(std::array<glm::vec4, 6>& planes, const BoundingSphere& boundingSphere);

    glm::vec2 HaltonSequence(int index, int baseX, int baseY);
}


#endif //OPENGLFRAMEWORK_SHAPES_H
