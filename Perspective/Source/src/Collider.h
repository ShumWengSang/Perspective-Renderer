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
#include "AABB.h"
struct Rigidbody;
struct Collider {
	// computed based on geometry
	float mass = 1.0f;
	glm::mat3 localInertiaTensor {1.0f};

	// Geometry related
	Rigidbody* rb = nullptr;

	// Position from aabb not used
	AABB aabb{};

	void SetShape(const AABB& other)
	{
		aabb = other;
		aabb.collider = this;
		auto [center, halfextent] = aabb.GetCenterHalfExtents();
		localInertiaTensor = glm::mat3 {
			mass * (halfextent.y * halfextent.y + halfextent.z * halfextent.z) / 3.0f, 0, 0,
			0, mass * (halfextent.x * halfextent.x + halfextent.z * halfextent.z) / 3.0f, 0,
			0, 0, mass * (halfextent.x * halfextent.x + halfextent.y * halfextent.y) / 3.0f
		};
	}

};
using ColliderPair = std::pair<Collider*, Collider*>;
using ColliderPairList = std::vector<ColliderPair>;
using ColliderList = std::vector<Collider>;