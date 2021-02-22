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
    enum class Type
    {
        Point3D = 0,
        Plane,
        Triangle,
        BoundingSphere,
        AABB,
        Ray,
        MAX
    };

    struct Shape
    {
        virtual ~Shape() = default;
        explicit Shape(Type type);
        Type type;
    };

    struct Point3D : public Shape
    {
        glm::vec3 coordinates;
        explicit Point3D(glm::vec3 const & point);

    };
    struct Plane : public Shape
    {
        // (n.x, n.y, n.z, d)
        glm::vec4 Normal;
        Plane(glm::vec3 const & normal, float d);
        explicit Plane(glm::vec4 const & normal);
    };
    struct Triangle : public Shape
    {
        glm::vec3 v1;
        glm::vec3 v2;
        glm::vec3 v3;
        Triangle(glm::vec3 const & a, glm::vec3 const & b, glm::vec3 const & c);
    };
    struct BoundingSphere : public Shape
    {
        glm::vec3 center;
        float radius;
        BoundingSphere(glm::vec3 const& center, float radius);
        BoundingSphere();
    };

    struct AABB : public Shape
    {
        glm::vec3 center;
        glm::vec3 halfExtents;
        AABB(glm::vec3 const & center, glm::vec3 const & halfExtents);
        AABB(glm::vec3 const & lowerBound, glm::vec3 const & upperBound, int empty);
        std::tuple<glm::vec3, glm::vec3> GetMinMax() const;
    };
    struct Ray : public Shape
    {
        glm::vec3 origin;
        glm::vec3 direction;
        Ray(glm::vec3 const & origin, glm::vec3 const & direction);
    };
    struct Collision
    {
        // Some collision information
        glm::vec3 pointA;
        glm::vec3 pointB;
    };

    bool CheckCollision(Shape const & a, Shape const & b, Collision& output);
    bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1);
    glm::vec3 Barycentric(const glm::vec3 &p, glm::vec3 const & a, glm::vec3 const & b, glm::vec3 const & c);

    template<typename T>
    inline bool isBetween(T const & obj, T const & a, T const & b)
    {
        return (a <= obj && obj <= b);
    }

    glm::vec3 SrgbColor(float r, float g, float b);

    void ExtractFrustumPlanes(const glm::mat4& matrix, std::array<glm::vec4, 6>& planes);
    void NormalizePlane(glm::vec4& plane);

    float VectorMaxComponent(const glm::vec3& vector);

    bool InPositiveHalfSpace(const glm::vec4& plane, const BoundingSphere& boundingSphere, float epsilon = 0.01f);
    bool InsideFrustum(std::array<glm::vec4, 6>& planes, const BoundingSphere& boundingSphere);

    glm::vec2 HaltonSequence(int index, int baseX, int baseY);
}


#endif //OPENGLFRAMEWORK_SHAPES_H
