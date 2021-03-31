//
// Created by user on 2/20/2021.
//

#include "stdafx.h"
#include "Shapes.h"
#include "Collision.h"
#include "Logger.h"
#include "ModelSystem.h"
#include "TransformSystem.h"

namespace Shapes {

    template<typename T, typename C>
    std::tuple<T const &, C const &> FixOrdering(Shape const &a, Shape const &b) {
        // If a != T
        if (dynamic_cast<T const *>(&a) == nullptr) {
            // Then b must be equal to T
            return std::tie(static_cast<T const &>(b), static_cast<C const &>(a));
        }
        return std::tie(static_cast<T const &>(a), static_cast<C const &>(b));
    }


    template<>
    bool CheckCollision<BoundingSphere, BoundingSphere>(Shape const &a, Shape const &b, Collision &output) {
        Log("Checking collision between a Sphere and a Sphere \n");
        Log("Passed in a %s and a %s\n", typeid(a).name(), typeid(b).name());
        const auto[SphereA, SphereB] = FixOrdering<BoundingSphere, BoundingSphere>(a, b);

        float totalRadiusSquared = (SphereA.radius + SphereB.radius) * (SphereA.radius + SphereB.radius);
        float distanceSquared = glm::distance2(SphereA.center, SphereB.center);

        return distanceSquared <= totalRadiusSquared;
    }

    template<>
    bool CheckCollision<BoundingSphere, AABB>(Shape const &a, Shape const &b, Collision &output) {
        Log("Checking collision between a Sphere and a Sphere \n");
        Log("Passed in a %s and a %s\n", typeid(a).name(), typeid(b).name());
        const auto[SphereA, Box] = FixOrdering<BoundingSphere, AABB>(a, b);

        const auto[min, max] = Box.GetMinMax();

        // Get closest point to sphere center
        glm::vec3 point = glm::max(min, glm::min(SphereA.center, max));

        float distance = glm::distance2(point, SphereA.center);

        return distance < SphereA.radius * SphereA.radius ;
    }

    template<>
    bool CheckCollision<AABB, AABB>(Shape const &a, Shape const &b, Collision &output) {
        Log("Checking collision between a Sphere and a Sphere \n");
        Log("Passed in a %s and a %s\n", typeid(a).name(), typeid(b).name());
        const auto[BoxA, BoxB] = FixOrdering<AABB, AABB>(a, b);

        const auto[minA , maxA] = BoxA.GetMinMax();
        const auto[minB , maxB] = BoxB.GetMinMax();

        for(unsigned i = 0; i < 3; ++i)
        {
            if(maxA[i] < minB[i] || maxB[i] < maxA[i])
                return false;
        }
        return true;
    }

    template<>
    bool CheckCollision<BoundingSphere, Point3D>(Shape const &a, Shape const &b, Collision &output) {
        const auto[SphereA, PointB] = FixOrdering<BoundingSphere, Point3D>(a, b);

        float radius2 = SphereA.radius * SphereA.radius;
        float distance2 = glm::distance2(SphereA.center, PointB.coordinates);
        return radius2 >= distance2;
    }

    template<>
    bool CheckCollision<AABB, Point3D>
            (Shape const &a, Shape const &b, Collision &output)
    {
        const auto[Box, Point] = FixOrdering<AABB, Point3D>(a, b);
        const auto[minA , maxA] = Box.GetMinMax();

        // 6 sig figs
        glm::vec3 rounded = glm::round(Point.coordinates * 100.f) / 100.f;
        bool ret = (rounded.x >= minA.x && rounded.x <= maxA.x) &&
                   (rounded.y >= minA.y && rounded.y <= maxA.y) &&
                   (rounded.z >= minA.z && rounded.z <= maxA.z);
        if(!ret)
        {
            //DEBUG_BREAKPOINT();
        }
        return ret;
    }

    template<>
    bool CheckCollision<Triangle, Point3D>
            (Shape const &a, Shape const &b, Collision &output)
    {
        const auto[triangle, Point] = FixOrdering<Triangle, Point3D>(a, b);
        glm::vec3 bary = Barycentric(Point.coordinates, triangle.v1, triangle.v2, triangle.v3);

        if(isBetween(bary.x, 0.0f, 1.0f) && isBetween(bary.y, 0.0f, 1.0f) && isBetween(bary.z, 0.0f, 1.0f))
        {
            return true;
        }
        return false;
    }

    template<>
    bool CheckCollision<Plane, Point3D>
            (Shape const &a, Shape const &b, Collision &output)
    {
        const auto[plane, point] = FixOrdering<Plane, Point3D>(a, b);
        if((plane.Normal.x * point.coordinates.x + plane.Normal.y * point.coordinates.y +
            plane.Normal.z * point.coordinates.z) == plane.Normal.w)
            return true;
        return false;
    }

    template<>
    bool CheckCollision<Ray, Plane>
            (Shape const &a, Shape const &b, Collision &output)
    {
        const auto[ray, plane] = FixOrdering<Ray, Plane>(a, b);
        glm::vec3 normal = glm::vec3(plane.Normal);
        float denom = glm::dot(normal, ray.direction);
        // Collinear, ray runs parallel to plane
        if(std::abs(denom) <= 1e-4f)
            return false;
        float t = -(glm::dot(normal, ray.origin) + plane.Normal.w) / glm::dot(normal, ray.direction);
        output.pointA = ray.origin + t * ray.direction;
        return true;
    }

    template<>
    bool CheckCollision<Ray, AABB>
            (Shape const &a, Shape const &b, Collision &output)
    {
        const auto[ray, aabb] = FixOrdering<Ray, AABB>(a, b);
        const auto[min, max] = aabb.GetMinMax();

        for(int i = 0; i < 3; ++i)
        {
            float invD = 1.0f / ray.direction[i];
            float t0 = (min[i] - ray.origin[i]) * invD;
            float t1 = (max[i] - ray.origin[i]) * invD;

            if(invD < 0.0f)
            {
                std::swap(t1, t0);
            }

            if(t0 <= t1)
            {
                return false;
            }
        }
        return true;
    }

    template<>
    bool CheckCollision<Ray, BoundingSphere>
            (Shape const &a, Shape const &b, Collision &output)
    {
        const auto[ray, sphere] = FixOrdering<Ray, BoundingSphere>(a, b);
        glm::vec3 L = ray.origin - sphere.center;
        float x = glm::dot(ray.direction, ray.direction);
        float y = 2 * glm::dot(ray.direction, L);
        float z = glm::dot(L,L) - sphere.radius * sphere.radius;
        float t0, t1; // solutions for t if the ray intersects
        if (!solveQuadratic(x, y, z, t0, t1)) return false;

        if(t0 > t1) std::swap(t0, t1);
        if (t0 < 0) {
            t0 = t1; // if t0 is negative, let's use t1 instead
            if (t0 < 0) return false; // both t0 and t1 are negative
        }

        return true;

    }

    template<>
    bool CheckCollision<Ray, Triangle>
            (Shape const &a, Shape const &b, Collision &output)
    {
        const auto[ray, triangle] = FixOrdering<Ray, Triangle>(a, b);

        // Create plane from 3 points
        glm::vec3 v1 (triangle.v2 - triangle.v1);
        glm::vec3 v2 (triangle.v3 - triangle.v1);
        glm::vec3 normal (glm::cross(v1,v2));
        Collision innerCollision = {};
        float d = glm::dot(normal, triangle.v1);
        Plane plane(glm::cross(v1,v2), d);

        if(CheckCollision<Ray, Plane>(ray, plane, output))
        {
            Collision innerinnerOutput;
            // Check if point is in triangle
            return CheckCollision<Triangle, Point3D>(triangle, Point3D(innerCollision.pointA), innerinnerOutput);
        }
        return false;
    }

    template<>
    bool CheckCollision<Plane, AABB>
            (Shape const &a, Shape const &b, Collision &output)
    {
        const auto[plane, aabb] = FixOrdering<Plane, AABB>(a, b);

        // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
        float r = aabb.halfExtents[0] * std::abs(plane.Normal[0]) +
                aabb.halfExtents[1] * std::abs(plane.Normal[1]) +
                aabb.halfExtents[2] * std::abs(plane.Normal[2]);

        // Compute distance of box center from plane
        float s = glm::dot(glm::vec3(plane.Normal), aabb.center) - plane.Normal.w;

        // Intersection occurs when distance s falls within [-r,+r] interval
        return std::abs(s) <= r;
    }

    template<>
    bool CheckCollision<Plane, BoundingSphere>
            (Shape const &a, Shape const &b, Collision &output)
    {
        const auto[plane, sphere] = FixOrdering<Plane, BoundingSphere>(a, b);
        // Project center of sphere on plane
        // Check if new point is within radius
        glm::vec3 p_prime = sphere.center -
                (glm::dot(glm::vec3(plane.Normal), sphere.center) + plane.Normal.w) * glm::vec3(plane.Normal);
        float radius2 = sphere.radius * sphere.radius;
        return glm::distance2(p_prime, sphere.center) <= radius2;
    }


    Point3D IntersectEdgeAgainstPlane(Point3D a, Point3D b, Plane plane) {
        glm::vec3 ab = b.coordinates - a.coordinates;

        float t = (plane.Normal.w - glm::dot(glm::vec3(plane.Normal), a.coordinates)) / (glm::dot(glm::vec3(plane.Normal), ab));

        if(t >= 0.0 && t <= 1.0f)
        {

        }
        else
        {
            Log("Uhhh");
        }
        Point3D res (a.coordinates + t * ab);
        return res;
    }

    void SuthHodgeClip(
            const std::vector<Shapes::Point3D> &polygon, Shapes::Plane const & plane
            , std::vector<Shapes::Point3D> &backFace, std::vector<Shapes::Point3D> &frontFace
                      ) {

        // Test all edges (a,b) starting with edger from last to first vertice
        Shapes::Point3D a = polygon.back();
        int aSide = Shapes::ClassifyPointToPlane(a, plane);

        backFace.clear();
        frontFace.clear();

        // Loop over all edges given by vertex pair (n - 1, n)
        for(int n = 0; n < polygon.size(); n++)
        {
            Point3D b = polygon[n];
            int bSide = Shapes::ClassifyPointToPlane(b, plane);

            if(bSide == POINT_IN_FRONT_OF_PLANE)
            {
                if(aSide == POINT_BEHIND_PLANE)
                {
                    // Edge (a,b) straddles output interaction point to both sides
                    Point3D i = IntersectEdgeAgainstPlane(a, b, plane);
                    //assert(ClassifyPointToPlane(i, plane) == POINT_ON_PLANE);
                    frontFace.emplace_back(i);
                    backFace.emplace_back(i);
                }
                frontFace.emplace_back(b);
            }
            else if(bSide == POINT_BEHIND_PLANE)
            {
                if(aSide == POINT_IN_FRONT_OF_PLANE)
                {
                    // Edge (a,b) straddles plane, output intersection point
                    Point3D i = IntersectEdgeAgainstPlane(b, a, plane);
                    //assert(ClassifyPointToPlane(i, plane) == POINT_ON_PLANE);
                    frontFace.emplace_back(i);
                    backFace.emplace_back(i);
                }
                else if(aSide == POINT_ON_PLANE)
                {
                    backFace.emplace_back(a);
                }
                backFace.emplace_back(b);
            }
            else
            {
                // B is on the plane, all three cases output b to front side
                frontFace.emplace_back(b);
                // In one case, b is also in backside
                if(aSide == POINT_BEHIND_PLANE)
                    backFace.emplace_back(b);
            }
            // Keep b as starting point of next edge
            a = b;
            aSide = bSide;
        }
    }

}

void CollisionMesh::AddModel(const LoadedModel &model) {
    const auto& model_vertices = model.vertices;
    for(int i = 0; i < model.indices.size(); i += 3)
    {
        int index1 = model.indices.at(i);
        int index2 = model.indices.at(i + 1);
        int index3 = model.indices.at(i + 2);
        Shapes::Triangle trig(model_vertices[index1].position, model_vertices[index2].position, model_vertices[index3].position);
        this->vertices.emplace_back(trig);
    }
    this->boundingBox = model.boundingBox;
}

void CollisionMesh::AddModel(const LoadedModel &model, int transformID) {
    Transform& transform = TransformSystem::getInstance().Get(transformID);

    const auto& model_vertices = model.vertices;

    for(int i = 0; i < model.indices.size(); i += 3)
    {
        int index1 = model.indices.at(i);
        int index2 = model.indices.at(i + 1);
        int index3 = model.indices.at(i + 2);
        Shapes::Triangle trig(
                glm::vec4(model_vertices[index1].position, 1) * transform.matrix,
                glm::vec4(model_vertices[index2].position, 1) * transform.matrix,
                glm::vec4(model_vertices[index3].position, 1) * transform.matrix);

        this->vertices.emplace_back(trig);
    }
    this->boundingBox = model.boundingBox;
}
