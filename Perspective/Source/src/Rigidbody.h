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
#include "Collider.h"

struct Rigidbody {
	float mass = 1.0f;
	float inverseMass = 1 / mass;

	glm::mat3 localInverseInertiaTensor = glm::mat3(1.0f);
	glm::mat3 globalInverseInertiaTensor = glm::mat3(1.0f);

	glm::vec3 position{};
	glm::quat orientation{1.0, 0, 0, 0};
	glm::quat inverseOrientation{ 1.0, 0, 0, 0 };

	glm::vec3 angularMomentum{};

	glm::vec3 linearVelocity{};
	glm::vec3 angularVelocity{};

	glm::vec3 forceAccumulator{};
	glm::vec3 torqueAccumulator{};

	float friction = 0.5f;
	float restitution = 0.5f;
	glm::vec3 halfExtent{};
	Collider collider;

	void UpdateOrientation(void);

	void AddCollider(Collider& collider);

	const glm::vec3 LocalToGlobal(const glm::vec3& p) const;
	const glm::vec3 GlobalToLocal(const glm::vec3& p) const;
	const glm::vec3 LocalToGlobalVec(const glm::vec3& v) const;
	const glm::vec3 GlobalToLocalVec(const glm::vec3& v) const;

	void ApplyForce(const glm::vec3& f);
	void ApplyForce(const glm::vec3& f, const glm::vec3& at);
	void ApplyImpulse(const glm::vec3& impulse, const glm::vec3 relativePosition);
	void UpdateVelocity(float dt);
	void UpdatePosition(float dt);
	void UpdateInvInertialWorld()
	{
		globalInverseInertiaTensor = glm::toMat3(orientation) * localInverseInertiaTensor * glm::toMat3(inverseOrientation);
	}
};
