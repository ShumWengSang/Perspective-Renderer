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


template <typename Writer>
inline void SerializeVec3(Writer& writer, glm::vec3 const & vector)
{
    writer.StartObject();
    writer.String(("x"));
    writer.Double(vector.x);
    writer.String(("y"));
    writer.Double(vector.y);
    writer.String(("z"));
    writer.Double(vector.z);
    writer.EndObject();
}

glm::vec3 DeserializeVec3(rapidjson::Value& value);


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

        Shape();
        Type type;
    };

    struct Point3D : public Shape
    {
        glm::vec3 coordinates;
        explicit Point3D(glm::vec3 const & point);

        bool operator== (Point3D const & rhs) const;
    };
    struct Plane : public Shape
    {
        // (n.x, n.y, n.z, d)
        glm::vec4 Normal;
        Plane(glm::vec3 const & normal, float d);
        Plane(glm::vec3 const & pointA, glm::vec3 const & pointB, glm::vec3 const & pointC);
        void Flip();
        explicit Plane(glm::vec4 const & normal);

        template<typename Writer>
        inline void Serialize(Writer& writer) const
        {
            writer.StartObject();
            writer.String(("Normal"));
            SerializeVec3(writer, glm::vec3(this->Normal));
            writer.String(("d"));
            writer.Double(Normal.w);
            writer.EndObject();
        }

        static Plane Deserialize(rapidjson::Value& val);
    };
    struct AABB : public Shape
    {
        glm::vec3 center = glm::vec3(0);
        glm::vec3 halfExtents  = glm::vec3(0);
        AABB(glm::vec3 const & center, glm::vec3 const & halfExtents);
        AABB(glm::vec3 const & lowerBound, glm::vec3 const & upperBound, int empty);
        AABB();
        std::tuple<glm::vec3, glm::vec3> GetMinMax() const;
        std::tuple<glm::vec3, glm::vec3> GetCenterHalfExtents() const;
        void RenderAABB(glm::vec4 const & color) const;

        template<typename Writer>
        inline void Serialize(Writer& writer) const
        {
            writer.StartObject();
            writer.String(("center"));
            SerializeVec3(writer, glm::vec3(this->center));
            writer.String(("halfExtents"));
            SerializeVec3(writer, glm::vec3(this->halfExtents));
            writer.EndObject();
        }

        static AABB Deserialize(rapidjson::Value& val);

    };
    struct Triangle : public Shape
    {
        glm::vec3 v1 = glm::vec3(0);
        glm::vec3 v2 = glm::vec3(0);
        glm::vec3 v3 = glm::vec3(0);
        Triangle(glm::vec3 const & a, glm::vec3 const & b, glm::vec3 const & c);
        Triangle();
        bool operator== (Triangle const & rhs) const;
        std::vector<Shapes::Triangle> BoundingVolumeCut(AABB const & boundingVolume) const;
        Shapes::Plane GetPlane() const;
        static Triangle Deserialize(rapidjson::Value& val);
        template<typename Writer>
        inline void Serialize(Writer& writer) const
        {
            writer.StartObject();
            writer.String(("v1"));
            SerializeVec3(writer, glm::vec3(this->v1));
            writer.String(("v2"));
            SerializeVec3(writer, glm::vec3(this->v2));
            writer.String(("v3"));
            SerializeVec3(writer, glm::vec3(this->v3));
            writer.EndObject();
        }
    };
    struct BoundingSphere : public Shape
    {
        glm::vec3 center;
        float radius;
        BoundingSphere(glm::vec3 const& center, float radius);
        BoundingSphere();
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

    inline const static int POINT_IN_FRONT_OF_PLANE = 1;
    inline const static int POINT_BEHIND_PLANE = -1;
    inline const static int POINT_ON_PLANE = 0;
    inline const static float PLANE_THICKNESS_EPISLON = 0.0001f;

    inline const static int POLYGON_STRADDLING_PLANE = 1;
    inline const static int POLYGON_IN_FRONT_OF_PLANE = 0;
    inline const static int POLYGON_BEHIND_PLANE = 2;
    inline const static int POLYGON_COPLANAR_WITH_PLANE = 3;

    int ClassifyPointToPlane(Point3D  const & p, Plane  const & plane);
    void
    SuthHodgeClip(const std::vector<Shapes::Point3D> &polygon, Shapes::Plane const & clipper,
            std::vector<Shapes::Point3D>& backFace, std::vector<Shapes::Point3D>& frontFace);
    int ClassifyPolyonToPlane(std::vector<Shapes::Point3D> const & polygon, Plane const & plane);
    std::vector<Shapes::Triangle> ReassemblePointsToTriangle(std::vector<Shapes::Point3D> const & polygon);
}
namespace std
{
    template<>
    struct hash<Shapes::Point3D>
    {
        size_t operator() (Shapes::Point3D const & obj) const;
    };
}

#endif //OPENGLFRAMEWORK_SHAPES_H
