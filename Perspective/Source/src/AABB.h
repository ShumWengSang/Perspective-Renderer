//
// Created by user on 1/24/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: App.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/24/2021
 * End Header --------------------------------------------------------*/
#pragma once
struct Collider;
struct AABB
{
    AABB() = default;
    AABB(glm::vec3 const& center, glm::vec3 const& halfExtents);
    AABB(glm::vec3 const& lowerBound, glm::vec3 const& upperBound, int empty);
    
    auto GetMinMax() const -> std::tuple<glm::vec3, glm::vec3>;
    auto GetCenterHalfExtents() const -> std::tuple<glm::vec3, glm::vec3>;

    glm::vec3 center = glm::vec3(0);
    glm::vec3 halfExtents = glm::vec3(0);
    Collider* collider = nullptr;

    static bool Collides(const AABB* a, const AABB* b);
    static bool Collides(const AABB& a, const AABB& b);
};
using AABBList = std::vector<AABB*>;