//
// Created by user on 1/25/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: Shapes.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/25/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "Shapes.h"
#include "Logger.h"
#include "Bitmask.h"
#include "Collision.h"

glm::vec3 Shapes::SrgbColor(float r, float g, float b) {
    glm::vec3 sRGB = glm::pow({ r, g, b }, glm::vec3(2.2f));
    return sRGB;
}

void Shapes::ExtractFrustumPlanes(const glm::mat4 &matrix, std::array<glm::vec4, 6> &planes) {
    // From the paper "Fast Extraction of Viewing Frustum Planes from the WorldView-Projection Matrix"
    // https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrixpdf

    // Left clipping plane
    planes[0].x = matrix[0][3] + matrix[0][0];
    planes[0].y = matrix[1][3] + matrix[1][0];
    planes[0].z = matrix[2][3] + matrix[2][0];
    planes[0].w = matrix[3][3] + matrix[3][0];

    // Right clipping plane
    planes[1].x = matrix[0][3] - matrix[0][0];
    planes[1].y = matrix[1][3] - matrix[1][0];
    planes[1].z = matrix[2][3] - matrix[2][0];
    planes[1].w = matrix[3][3] - matrix[3][0];

    // Top clipping plane
    planes[2].x = matrix[0][3] - matrix[0][1];
    planes[2].y = matrix[1][3] - matrix[1][1];
    planes[2].z = matrix[2][3] - matrix[2][1];
    planes[2].w = matrix[3][3] - matrix[3][1];

    // Bottom clipping plane
    planes[3].x = matrix[0][3] + matrix[0][1];
    planes[3].y = matrix[1][3] + matrix[1][1];
    planes[3].z = matrix[2][3] + matrix[2][1];
    planes[3].w = matrix[3][3] + matrix[3][1];

    // Near clipping plane
    planes[4].x = matrix[0][3] + matrix[0][2];
    planes[4].y = matrix[1][3] + matrix[1][2];
    planes[4].z = matrix[2][3] + matrix[2][2];
    planes[4].w = matrix[3][3] + matrix[3][2];

    // Far clipping plane
    planes[5].x = matrix[0][3] - matrix[0][2];
    planes[5].y = matrix[1][3] - matrix[1][2];
    planes[5].z = matrix[2][3] - matrix[2][2];
    planes[5].w = matrix[3][3] - matrix[3][2];

    // Normalize planes (so that we can measure actual distances)
    NormalizePlane(planes[0]);
    NormalizePlane(planes[1]);
    NormalizePlane(planes[2]);
    NormalizePlane(planes[3]);
    NormalizePlane(planes[4]);
    NormalizePlane(planes[5]);
}

void Shapes::NormalizePlane(glm::vec4 &plane) {
    float length = std::sqrt(plane.x * plane.x + plane.y * plane.y + plane.z * plane.z);
    plane *= 1.0f / length;
}

float Shapes::VectorMaxComponent(const glm::vec3 &vector) {
    return std::max(std::max(vector.x, vector.y), vector.z);
}

bool Shapes::InPositiveHalfSpace(const glm::vec4 &plane, const Shapes::BoundingSphere &boundingSphere, float epsilon) {
    float signedDistance = glm::dot(plane, glm::vec4(boundingSphere.center, 1.0f));
    return signedDistance + boundingSphere.radius + epsilon >= 0.0f;
}

bool Shapes::InsideFrustum(std::array<glm::vec4, 6> &planes, const Shapes::BoundingSphere &boundingSphere) {
    if (!InPositiveHalfSpace(planes[0], boundingSphere)) return false;
    if (!InPositiveHalfSpace(planes[1], boundingSphere)) return false;
    if (!InPositiveHalfSpace(planes[2], boundingSphere)) return false;
    if (!InPositiveHalfSpace(planes[3], boundingSphere)) return false;
    if (!InPositiveHalfSpace(planes[4], boundingSphere)) return false;
    if (!InPositiveHalfSpace(planes[5], boundingSphere)) return false;
    return true;
}
#include <new>
glm::vec2 Shapes::HaltonSequence(int index, int baseX, int baseY) {
// Code is a modified version of the HaltonSequence sequence generation from
    // https://blog.demofox.org/2017/05/29/when-random-numbers-are-too-random-low-discrepancy-sequences/

    glm::vec2 sample{};

    // x axis
    {
        float denominator = float(baseX);
        size_t n = index;
        while (n > 0)
        {
            size_t multiplier = n % baseX;
            sample.x += float(multiplier) / denominator;
            n = n / baseX;
            denominator *= baseX;
        }
    }

    // y axis
    {
        float denominator = float(baseY);
        size_t n = index;
        while (n > 0)
        {
            size_t multiplier = n % baseY;
            sample.y += float(multiplier) / denominator;
            n = n / baseY;
            denominator *= baseY;
        }
    }

    return sample;
}



Shapes::Ray::Ray(glm::vec3 const & origin, glm::vec3 const & direction) : origin(origin),
    direction(direction), Shape(Type::Ray){

}

Shapes::Triangle::Triangle(glm::vec3 const & a, glm::vec3 const & b, glm::vec3 const & c) :
    v1(a), v2(b), v3(c), Shape(Type::Triangle){

}

Shapes::Plane::Plane(glm::vec3 const & normal, float d) :Normal(normal, d), Shape(Type::Plane){

}

Shapes::Plane::Plane(glm::vec4 const & normal) :Normal(normal), Shape(Type::Plane){

}

Shapes::Point3D::Point3D(glm::vec3 const & point) : coordinates(point), Shape(Type::Point3D){

}

Shapes::AABB::AABB(glm::vec3 const & center, glm::vec3 const & halfExtents) : center(center), halfExtents(halfExtents),
    Shape(Type::AABB){

}

Shapes::AABB::AABB(glm::vec3 const & lowerBound, glm::vec3 const & upperBound, int nothing) :
    center((lowerBound.x + upperBound.x) / 2.0f, (lowerBound.y + upperBound.y) / 2.0f, (lowerBound.z + upperBound.z) / 2.0f),
    halfExtents((upperBound - lowerBound) / 2.0f), Shape(Type::AABB){
    (void)nothing;
}

std::tuple<glm::vec3, glm::vec3> Shapes::AABB::GetMinMax() const {
    glm::vec3 max = center + halfExtents;
    glm::vec3 min = center - halfExtents;
    return std::make_tuple(max, min);
}

Shapes::BoundingSphere::BoundingSphere(glm::vec3 const& center, float radius) :
    center(center), radius(radius), Shape(Type::BoundingSphere){

}

Shapes::BoundingSphere::BoundingSphere() : BoundingSphere(glm::vec3(0), 0){

}

Shapes::Shape::Shape(Shapes::Type type) : type(type){

}

bool Shapes::CheckCollision(const Shapes::Shape &a, const Shapes::Shape &b, Shapes::Collision &output) {
    Bitmask<Shapes::Type> currMask = Bitmask<Shapes::Type>(a.type) | b.type;

    // Basic
    static const Bitmask<Shapes::Type> Sphere_Sphere = Shapes::Type::BoundingSphere;
    static const Bitmask<Shapes::Type> Sphere_AABB = Type::BoundingSphere | Type::AABB;
    static const Bitmask<Shapes::Type> AABB_AABB = Type::AABB;

    // Point
    static const Bitmask<Shapes::Type> Point_Sphere     = Type::Point3D | Type::BoundingSphere;
    static const Bitmask<Shapes::Type> Point_AABB = Type::Point3D | Type::AABB;
    static const Bitmask<Shapes::Type> Point_Triangle = Type::Point3D | Type::Triangle;
    static const Bitmask<Shapes::Type> Point_Plane  = Type::Point3D | Type::Plane;

    // Ray-Based
    static const Bitmask<Shapes::Type> Ray_Sphere   = Type::Ray | Type::BoundingSphere;
    static const Bitmask<Shapes::Type> Ray_AABB     = Type::Ray | Type::AABB;
    static const Bitmask<Shapes::Type> Ray_Triangle = Type::Ray | Type::Triangle;
    static const Bitmask<Shapes::Type> Ray_Plane    = Type::Ray | Type::Plane;

    // Plane-based
    static const Bitmask<Shapes::Type> Plane_Sphere  = Type::Plane | Type::BoundingSphere;
    static const Bitmask<Shapes::Type> Plane_AABB    = Type::Plane | Type::AABB;

    if(currMask == Sphere_Sphere)
    {
        return CheckCollisionParameterAgnostic<BoundingSphere, BoundingSphere>(a, b, output);
    }
    else if(currMask == Sphere_AABB)
    {
        return CheckCollisionParameterAgnostic<BoundingSphere, AABB>(a, b, output);
    }else if(currMask == AABB_AABB)
    {
        return CheckCollisionParameterAgnostic<AABB, AABB>(a, b, output);
    }else if(currMask == Point_Sphere)
    {
        return CheckCollisionParameterAgnostic<BoundingSphere, Point3D>(a, b, output);
    }else if(currMask == Point_AABB)
    {
        return CheckCollisionParameterAgnostic<AABB, Point3D>(a, b, output);
    }else if(currMask == Point_Triangle)
    {
        return CheckCollisionParameterAgnostic<Triangle, Point3D>(a, b, output);
    }else if(currMask == Point_Plane)
    {
        return CheckCollisionParameterAgnostic<Plane, Point3D>(a, b, output);
    }else if(currMask == Ray_Sphere)
    {
        return CheckCollisionParameterAgnostic<Ray, BoundingSphere>(a, b, output);
    }else if(currMask == Ray_AABB)
    {
        return CheckCollisionParameterAgnostic<Ray, AABB>(a, b, output);
    }else if(currMask == Ray_Triangle)
    {
        return CheckCollisionParameterAgnostic<Ray, Triangle>(a, b, output);
    }else if(currMask == Ray_Plane)
    {
        return CheckCollisionParameterAgnostic<Ray, Plane>(a, b, output);
    }else if(currMask == Plane_Sphere)
    {
        return CheckCollisionParameterAgnostic<Plane, BoundingSphere>(a, b, output);
    }else if(currMask == Plane_AABB)
    {
        return CheckCollisionParameterAgnostic<Plane, AABB>(a, b, output);
    }
    else
    {
        LogError("Error! currMask not identified\n");
        return false;
    }
}

glm::vec3 Shapes::Barycentric(const glm::vec3 &p, const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c) {
    glm::vec3 vwu;
    // Cramer's Rule
    glm::vec3 v0 = b - a, v1 = c - a, v2 = p - a;
    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;

    vwu.y = (d11 * d20 - d01 * d21) / denom;
    vwu.z = (d00 * d21 - d01 * d20) / denom;
    vwu.x = 1.0f - vwu.y - vwu.z;
    return vwu;
}

bool Shapes::solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1) {
    float discr = b * b - 4 * a * c;
    if (discr < 0) return false;
    else if (discr == 0) x0 = x1 = - 0.5 * b / a;
    else {
        float q = (b > 0) ?
                  -0.5 * (b + sqrt(discr)) :
                  -0.5 * (b - sqrt(discr));
        x0 = q / a;
        x1 = c / q;
    }
    if (x0 > x1) std::swap(x0, x1);

    return true;
}
