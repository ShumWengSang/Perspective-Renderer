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
#include "Rigidbody.h"

struct Joint
{
	Joint() = default;

	void Set(Rigidbody* body1, Rigidbody* body2, const glm::vec3& anchor);

	void PreStep(float inv_dt);
	void ApplyImpulse();

	glm::mat3 M;
	glm::vec3 localAnchor1{}, localAnchor2{};
	glm::vec3 r1{}, r2{};
	glm::vec3 bias{};
	glm::vec3 P {};		// accumulated impulse
	Rigidbody* body1 = nullptr;
	Rigidbody* body2 = nullptr;
	float biasFactor = 0.2f;
	float softness = 0.0f;
};