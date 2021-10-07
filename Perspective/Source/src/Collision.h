//
// Created by user on 2/19/2021.
//

#ifndef OPENGLFRAMEWORK_COLLISION_H
#define OPENGLFRAMEWORK_COLLISION_H

struct LoadedModel;


namespace Shapes {


    template<typename ShapeA, typename ShapeB>
    inline bool CheckCollision(Shape const &a, Shape const &b, Collision &output) {
        Log("Warning! Collision between %s and %s are not implemented!\n", typeid(a).name(), typeid(b).name());
        return false;
    }

    Point3D IntersectEdgeAgainstPlane(Point3D a, Point3D b, Plane plane);


    template<>
    bool CheckCollision<BoundingSphere, BoundingSphere>
            (Shape const &a, Shape const &b, Collision &output);

    template<>
    bool CheckCollision<BoundingSphere, AABB>
            (Shape const &a, Shape const &b, Collision &output);

    template<>
    bool CheckCollision<AABB, AABB>
            (Shape const &a, Shape const &b, Collision &output);

    template<>
    bool CheckCollision<BoundingSphere, Point3D>
            (Shape const &a, Shape const &b, Collision &output);

    template<>
    bool CheckCollision<AABB, Point3D>
            (Shape const &a, Shape const &b, Collision &output);

    template<>
    bool CheckCollision<Triangle, Point3D>
            (Shape const &a, Shape const &b, Collision &output);

    template<>
    bool CheckCollision<Plane, Point3D>
            (Shape const &a, Shape const &b, Collision &output);

    template<>
    bool CheckCollision<Ray, Plane>
            (Shape const &a, Shape const &b, Collision &output);

    template<>
    bool CheckCollision<Ray, AABB>
            (Shape const &a, Shape const &b, Collision &output);

    template<>
    bool CheckCollision<Ray, BoundingSphere>
            (Shape const &a, Shape const &b, Collision &output);

    template<>
    bool CheckCollision<Ray, Triangle>
            (Shape const &a, Shape const &b, Collision &output);

    template<>
    bool CheckCollision<Plane, BoundingSphere>
            (Shape const &a, Shape const &b, Collision &output);

    template<>
    bool CheckCollision<Plane, AABB>
            (Shape const &a, Shape const &b, Collision &output);


    template<typename ShapeA, typename ShapeB>
    bool CheckCollisionParameterAgnostic(Shape const &a, Shape const &b, Collision &output) {
        static constexpr auto DefaultFallbackFunc = &CheckCollision<int, int>;
        if (&CheckCollision<ShapeA, ShapeB> == DefaultFallbackFunc) // If there is no generated func
        {
            // Attempt to swap the parameters, and then call
            return CheckCollision<ShapeB, ShapeA>(a, b, output);
        }
        return CheckCollision<ShapeA, ShapeB>(a, b, output);
    }


}

class CollisionMesh {
public:
    CollisionMesh() = default;

    void AddModel(LoadedModel const &model);

    void AddModel(LoadedModel const &model, int transformID);

private:
    std::vector <Shapes::Triangle> vertices;
    Shapes::AABB boundingBox;

    friend class OctTree;

    friend class BSPTree;
};


#endif //OPENGLFRAMEWORK_COLLISION_H
