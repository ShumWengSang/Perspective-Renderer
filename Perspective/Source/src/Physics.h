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
#include "Contact.h"
#include "Rigidbody.h"
#include "N_SquareBroadphase.h"
class Physics
{
public:
	void Update(float dt, int iterations);
	bool RegisterRigidbody(Rigidbody* rb);
private:
	std::vector<Rigidbody*> rigidbodies;
	std::vector<Contact> contacts;
	std::map<ArbiterKey, Arbiter> arbiters;
	NSquared broadphase;
	rbSolver solver;
};