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
#include "rbSolver.h"

# define PHYSICS_TOLERANCE (1e-6f)
# define PHYSICS_REAL_MAX FLT_MAX



struct Contact {
    glm::vec3 Position;
    glm::vec3 RelativeBodyPosition[2];
    Rigidbody* Body[2];

    // direction of Normal is always from Body[1] to Body[0]
    glm::vec3 Normal;

    float PenetrationDepth;

    Contact() = default;

    Contact(const Contact& other)
        : Position(other.Position)
        , RelativeBodyPosition{ other.RelativeBodyPosition[0], other.RelativeBodyPosition[1] }
        , Body{ other.Body[0], other.Body[1] }
        , Normal(other.Normal)
        , PenetrationDepth(other.PenetrationDepth)
    {}

    Contact& operator =(const Contact& other)
    {
        if (this != &other)
        {
            Position = other.Position;
            RelativeBodyPosition[0] = other.RelativeBodyPosition[0];
            RelativeBodyPosition[1] = other.RelativeBodyPosition[1];
            Body[0] = other.Body[0];
            Body[1] = other.Body[1];
            Normal = other.Normal;
            PenetrationDepth = other.PenetrationDepth;
        }

        return *this;
    }

    bool operator ==(const Contact& other) const
    {
        if (this != &other)
        {
            return Position == other.Position &&
            RelativeBodyPosition[0] == other.RelativeBodyPosition[0] &&
            RelativeBodyPosition[1] == other.RelativeBodyPosition[1] &&
            Body[0] == other.Body[0] &&
            Body[1] == other.Body[1] &&
            Normal == other.Normal &&
            PenetrationDepth == other.PenetrationDepth;
        }

        return true;
    }
};

namespace NarrowCollision
{
    bool Detect(Rigidbody* box0, Rigidbody* box1, std::vector<Contact>& contacts_out);
}
struct ArbiterKey
{
    ArbiterKey(Rigidbody* b1, Rigidbody* b2)
    {
        if (b1 < b2)
        {
            body1 = b1; body2 = b2;
        }
        else
        {
            body1 = b2; body2 = b1;
        }
    }

    Rigidbody* body1;
    Rigidbody* body2;
};
struct Arbiter
{
    Arbiter(Rigidbody* a, Rigidbody* b);
    void Update(const std::vector<Contact>& newContacts);
    void PreStep(float inv_dt);
    void ApplyImpulse();

    std::vector<Contact> contacts;
    Rigidbody* body1 = nullptr;
    Rigidbody* body2 = nullptr;

    // Combined friction
    float friction;

    void ApplySolver(rbSolver* solver, float dt);

    bool operator==(const Arbiter& other) const
    {
        if (other.friction != friction)
            return false;
        if (other.contacts.size() != contacts.size())
            return false;
        if (body1 != other.body1 || body2 != other.body2)
            return false;
        for (int i = 0; i < contacts.size(); ++i)
        {
            if (!(other.contacts[i] == this->contacts[i]))
                return false;
        }

        return true;
    }

};

// This is used by std::set
bool operator< (const ArbiterKey& a1, const ArbiterKey& a2);
