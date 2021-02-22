//
// Created by user on 2/20/2021.
//
#include "AssignmentTwo.h"
#include "Shapes.h"
#include "Logger.h"

// True to pass, false to break.
#define MyAssert(expr) if(!expr) DEBUG_BREAKPOINT();

void AssignmentTwo::RunTestCases() {
    using namespace Shapes;
    Shapes::Collision collisionInfo;

    {
        // Sphere vs Sphere not colliding
        BoundingSphere a(glm::vec3(0), 10.0);
        BoundingSphere b(glm::vec3(20), 10.0);
        MyAssert(CheckCollision(a, b, collisionInfo) == false);
    }
    {
        // Sphere vs Sphere colliding
        BoundingSphere a(glm::vec3(0), 10.0);
        BoundingSphere b(glm::vec3(10,0,0), 10.0);
        MyAssert(CheckCollision(a, b, collisionInfo) == true);
    }
    {
        // Sphere and AABB Not colliding
        BoundingSphere a(glm::vec3(0), 10.0);
        AABB b(glm::vec3(20,0,0), glm::vec3(5,5,5));
        MyAssert(CheckCollision(a, b, collisionInfo) == false);
    }
    {
        // Sphere and AABB colliding
        BoundingSphere a(glm::vec3(0), 10.0);
        AABB b(glm::vec3(10,0,0), glm::vec3(10,10,10));
        MyAssert(CheckCollision(a, b, collisionInfo) == true);
    }
    {
        // AABB not colliding
        AABB a(glm::vec3(0), glm::vec3(5,5,5));
        AABB b(glm::vec3(20,0,0), glm::vec3(10,10,10));
        MyAssert(CheckCollision(a, b, collisionInfo) == false);
    }
    {
        // AABB Colliding
        AABB a(glm::vec3(0), glm::vec3(10,10,10));
        AABB b(glm::vec3(10,0,0), glm::vec3(10,10,10));
        MyAssert(CheckCollision(a, b, collisionInfo) == true);
    }
    {
        // Point Sphere not Colliding
        Point3D point(glm::vec3(0));
        BoundingSphere sphere(glm::vec3(10, 0,0), 9.0f);
        MyAssert(CheckCollision(point, sphere, collisionInfo) == false);
    }
    {
        // Point Sphere Colliding
        Point3D point(glm::vec3(0));
        BoundingSphere sphere(glm::vec3(10, 0,0), 10.0f);
        MyAssert(CheckCollision(point, sphere, collisionInfo) == true);
    }
    {
        // Point AABB not colliding
        Point3D point(glm::vec3(-1));
        AABB box(glm::vec3(10, 0,0), glm::vec3(10,10,10));
        MyAssert(CheckCollision(point, box, collisionInfo) == false);
    }
    {
        // Point AABB colliding
        Point3D point(glm::vec3(0));
        AABB box(glm::vec3(5, 0,0), glm::vec3(10,10,10));
        MyAssert(CheckCollision(point, box, collisionInfo) == true);
    }
    {
        // Point Triangle not colliding
        Point3D point(glm::vec3(1,1,1));
        Triangle triangle(glm::vec3(-1,0,0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
        MyAssert(CheckCollision(point, triangle, collisionInfo) == false);
    }
    {
        // Point triangle colliding
        Point3D point(glm::vec3(0));
        Triangle triangle(glm::vec3(-1,0,0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1));
        MyAssert(CheckCollision(point, triangle, collisionInfo) == true);
    }
    {
        // Point plane not colliding
        Point3D point(glm::vec3(1));
        Plane plane(glm::vec3(1,1,1),0);
        MyAssert(CheckCollision(point, plane, collisionInfo) == false);
    }
    {
        //Point plane colliding
        Point3D point(glm::vec3(1));
        Plane plane(glm::vec3(1,1,1),3);
        MyAssert(CheckCollision(point, plane, collisionInfo) == true);
    }

    Log("All tests passed!\n");
}
