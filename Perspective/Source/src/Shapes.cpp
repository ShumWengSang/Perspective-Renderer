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

namespace std {
    size_t std::hash<Shapes::Point3D>::operator()(const Shapes::Point3D &obj) const {
        return hash<glm::vec3>()(obj.coordinates);
    }
}

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

// Return triangle + generated triangle (if necessary) for within bounds
std::vector<Shapes::Triangle> Shapes::Triangle::BoundingVolumeCut(const Shapes::AABB &boundingVolume) const {
    auto [Min, Max] = boundingVolume.GetMinMax();

    // Generate 6 planes pointing inward into the box, run the SH algo to clip triangle against all planes
    std::array<Shapes::Plane, 6> planes = {
            Shapes::Plane(Max, glm::vec3(Max.x, Max.y, Min.z), glm::vec3(Max.x, Min.y, Min.z)), // Left Face
            Shapes::Plane(Min, glm::vec3(Max.x, Min.y, Min.z), glm::vec3(Max.x, Max.y, Min.z)), // Front
            Shapes::Plane(Min, glm::vec3(Min.x, Max.y, Min.z), glm::vec3(Min.x, Max.y, Max.z)),
            Shapes::Plane(Max, glm::vec3(Max.x, Min.y, Max.z), glm::vec3(Min.x, Min.y, Max.z)),
            Shapes::Plane(Max,glm::vec3(Min.x, Max.y, Max.z), glm::vec3(Min.x, Max.y, Min.z)),
            Shapes::Plane(Min,glm::vec3(Min.x, Min.y, Max.z), glm::vec3(Max.x, Min.y, Max.z))
    };


    std::vector<Shapes::Point3D> polygon = {
            Shapes::Point3D(this->v1), Shapes::Point3D(this->v2), Shapes::Point3D(this->v3)
    };
    std::vector<Shapes::Point3D> resultantPolygon;
    std::vector<Shapes::Point3D> frontFace;
    std::vector<Shapes::Point3D> backFace;
    for(int i = 0; i < 6; ++i)
    {
        if(polygon.empty())
            break;
        // planes[i].Flip();
        // Clip the results
        SuthHodgeClip(polygon, planes[i], backFace, frontFace);
        // Set the output of the clipping algo as the new polygon to cut against
        polygon = frontFace;
        backFace.clear();
        frontFace.clear();
    }

    return Shapes::ReassemblePointsToTriangle(polygon);
}

bool Shapes::Triangle::operator==(const Shapes::Triangle &rhs) const {
    return this->v1 == rhs.v1 && this->v2 == rhs.v2 && this->v3 == rhs.v3;
}

Shapes::Triangle Shapes::Triangle::Deserialize(rapidjson::Value &val) {

    Shapes::Triangle res (
    DeserializeVec3(val["v1"]),
    DeserializeVec3(val["v2"]),
    DeserializeVec3(val["v3"]));
    return res;
}

Shapes::Triangle::Triangle() : Shape(Type::Plane) {

}

Shapes::Plane Shapes::Triangle::GetPlane() const {
    return Shapes::Plane(v1, v2, v3);
}

Shapes::Plane::Plane(glm::vec3 const & normal, float d) : Shape(Type::Plane){
    float mag = glm::length(normal);
    Normal = glm::vec4(normal, d) / mag;
}

Shapes::Plane::Plane(glm::vec4 const & normal) :Normal(normal), Shape(Type::Plane){

}

Shapes::Plane::Plane(glm::vec3 const & pointA, glm::vec3 const & pointB, glm::vec3 const & pointC) : Shape(Type::Plane)
{
    glm::vec3 AB = pointB - pointA;
    glm::vec3 AC = pointC - pointA;

    glm::vec3 normal = glm::normalize(glm::cross(AB, AC));
    float d = glm::dot(pointA, normal);
    Normal = glm::vec4(normal, d);
}

void Shapes::Plane::Flip() {
    glm::vec3 norm = glm::vec3(this->Normal);
    float d = this->Normal.w;
    norm *= -1;
    Normal = glm::vec4(norm, d);
}

Shapes::Plane Shapes::Plane::Deserialize(rapidjson::Value &val) {
    glm::vec3 normal = DeserializeVec3(val["Normal"]);
    float d = val["d"].GetFloat();
    return Plane(normal, d);
}

Shapes::Point3D::Point3D(glm::vec3 const & point) : coordinates(point), Shape(Type::Point3D){

}

bool Shapes::Point3D::operator==(Point3D const & rhs) const{
    return this->coordinates == rhs.coordinates;
}

Shapes::AABB::AABB(glm::vec3 const & center, glm::vec3 const & halfExtents) : center(center), halfExtents(halfExtents),
    Shape(Type::AABB){

}

Shapes::AABB::AABB(glm::vec3 const & lowerBound, glm::vec3 const & upperBound, int nothing) :
    center((lowerBound + upperBound) / 2.0f),
    halfExtents((upperBound - lowerBound) / 2.0f), Shape(Type::AABB){
    (void)nothing;
}

std::tuple<glm::vec3, glm::vec3> Shapes::AABB::GetMinMax() const {
    glm::vec3 max = center + halfExtents;
    glm::vec3 min = center - halfExtents;
    return std::make_tuple(min, max);
}

std::tuple<glm::vec3, glm::vec3> Shapes::AABB::GetCenterHalfExtents() const{
    return std::make_tuple(center, halfExtents);
}

Shapes::AABB::AABB() : Shape(Type::AABB){

}

void Shapes::AABB::RenderAABB(glm::vec4 const & color) const {
    auto [Min, Max] = this->GetMinMax();
    dd::aabb(glm::value_ptr(Min), glm::value_ptr(Max), glm::value_ptr(color));
}

Shapes::AABB Shapes::AABB::Deserialize(rapidjson::Value &val) {
    Shapes::AABB res( DeserializeVec3(val["center"]), DeserializeVec3(val["halfExtents"]));
    return res;
}

Shapes::BoundingSphere::BoundingSphere(glm::vec3 const& center, float radius) :
    center(center), radius(radius), Shape(Type::BoundingSphere){

}

Shapes::BoundingSphere::BoundingSphere() : BoundingSphere(glm::vec3(0), 0){

}

Shapes::Shape::Shape(Shapes::Type type) : type(type){

}

Shapes::Shape::Shape() {}


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

int Shapes::ClassifyPointToPlane(Shapes::Point3D  const & p, Shapes::Plane const & plane) {
    float dist = glm::dot(glm::vec3(plane.Normal), p.coordinates) - plane.Normal.w;

    if(dist > PLANE_THICKNESS_EPISLON)
        return POINT_IN_FRONT_OF_PLANE;
    if(dist < -PLANE_THICKNESS_EPISLON)
        return POINT_BEHIND_PLANE;
    return POINT_ON_PLANE;
}

int Shapes::ClassifyPolyonToPlane(const std::vector<Shapes::Point3D> &polygon, const Shapes::Plane &plane) {
    int numInFront = 0, numBehind = 0;
    for(int i = 0; i < polygon.size(); i++)
    {
        const Shapes::Point3D& p = polygon[i];
        switch(ClassifyPointToPlane(p, plane)){
            case POINT_IN_FRONT_OF_PLANE:
                numInFront++;
                break;
            case POINT_BEHIND_PLANE:
                numBehind++;
                break;
        }
    }
    // See where vertices are
    // Polygon is straddling
    if(numBehind != 0 && numInFront != 0)
    {
        return POLYGON_STRADDLING_PLANE;
    }
    // No vertices behind, all in front
    else if(numInFront != 0)
    {
        return POLYGON_IN_FRONT_OF_PLANE;
    }
    // No vertices in front, all behind
    else if(numBehind != 0)
    {
        return POLYGON_BEHIND_PLANE;
    }
    else
    {
        // All vertices lie on plane, so is coplaner
        return POLYGON_COPLANAR_WITH_PLANE;
    }
}

std::vector<Shapes::Triangle> Shapes::ReassemblePointsToTriangle(const std::vector<Shapes::Point3D> &polygon) {
    // Now from all the points we reassembly the triangles
    static std::vector<Shapes::Triangle> res;
    res.clear();
    res.reserve(polygon.size() / 3);

    for(int i = 2; i < polygon.size(); ++i)
    {
        res.emplace_back(Shapes::Triangle(polygon[0].coordinates,
                                          polygon[i - 1].coordinates,
                                          polygon[i].coordinates));
    }
    return res;
}

glm::vec3 DeserializeVec3(rapidjson::Value &value) {
    glm::vec3 result;
    result.x = value["x"].GetFloat();
    result.y = value["y"].GetFloat();
    result.z = value["z"].GetFloat();
    return result;
}
