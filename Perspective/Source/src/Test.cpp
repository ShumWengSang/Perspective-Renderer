//
// Created by user on 4/18/2021.
//

// DreamWorks Animation LLC Confidential Information.
// TM and (c) 2017-2021 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.

/// @file MoonrayPA.cc
/// @author Moonray Dev Team

// The purpose of this exercise is to provide some insight into how you write
// code and solve problems.  The assignment is intended to take around
// four hours, but feel free to take as long as you need.  If you don’t
// solve a problem completely, please provide as much of a partial solution
// as possible for us to evaluate.  The assignment is not pass/fail.
// It is better to provide an answer that demonstrates an
// understanding of the key issues, rather than no answer at all.
//
// We will compile this file and run it against our regression test suite.  You
// can search for the string "TODO:" to see all the areas of the file you should
// edit.
//


// TODO: add any needed header files

#include <vector>
#include <cmath>     // std::hypot
#include <iostream>  // cout and debug
#include <algorithm> // set_difference
#include <numeric>   // accumulate

// Preliminaries
// In the following problems, you can use the following single precision
// vector type. Fill in any methods or operators that you need.
class Vec3f {
public:
    float x, y, z;

    // TODO: Add needed Vec3f methods here
    constexpr float Dot(const Vec3f &other) const {
        return this->x * other.x + this->y * other.y + this->z * other.z;
    }

    constexpr float DotSelf() const {
        return this->Dot(*this);
    }

    float Length() const {
        return std::hypot(x, y, z);
    }

    constexpr Vec3f Cross(const Vec3f &other) const {
        return Vec3f{this->y * other.z - this->z * other.y,
                     this->z * other.x - this->x * other.z,
                     this->x * other.y - this->y * other.x};
    }
};

// TODO: Add needed Vec3f operators here
constexpr Vec3f operator/(const Vec3f &left, float right) {
    return Vec3f{left.x / right, left.y / right, left.z / right};
}

constexpr Vec3f operator*(const Vec3f &left, float right) {
    return Vec3f{left.x * right, left.y * right, left.z * right};
}

constexpr Vec3f operator*(float left, const Vec3f &right) {
    return right * left;
}

constexpr Vec3f operator-(const Vec3f &left, const Vec3f &right) {
    return Vec3f{left.x - right.x, left.y - right.y, left.z - right.z};
}

constexpr Vec3f operator+(const Vec3f &left, const Vec3f &right) {
    return Vec3f{left.x + right.x, left.y + right.y, left.z + right.z};
}

constexpr Vec3f operator-(const Vec3f &v) {
    return Vec3f{-v.x, -v.y, -v.z};
}

std::ostream &operator<<(std::ostream &os, const Vec3f &vec) {
    os << "[" << vec.x << "," << vec.y << "," << vec.z << "]";
    return os;
}

// Problem 1: Normalize a Vec3f
// Given a Vec3f vector of arbitrary length, write a function that returns
// a normalized version of that vector.  Consider the possibility that
// the vector might be very large or very small.  The use of double precision
// is discouraged.
Vec3f normalize(const Vec3f &n) {
    // Naive solution of x*x + y*y + z*z would have underflow overflow issues
    // The C library has something for this
    // called hypot, which is a slower hypothenuse function
    // that does not overflow or underflow.
    // So only one float divide is used post hypot()
    // The idea is to transform the input values from sqrt(a^2 + b^2) == |b| * sqrt((a/b)^2 + 1)
    // where |b| >= |a|, and a/b <= 1. Thus (a/b)^2 won't overflow, and if it underflow it doesn't matter
    // since we add 1

    return n / n.Length();
}


// Problem 2: Intersect a ray with a sphere
// Please fill in the details of the following function.
// The function should return true if the ray defined
// by rayOrigin, and rayDir intersects the sphere located at
// sphereLoc with sphereRadius.  If the ray intersects the
// sphere, then the location of this intersection should be written
// in the hitPoint parameter.
//
// A common production use of spheres is to represent tiny particles.
// So your solution should handle the case where the distance from
// the ray origin may be quite a bit larger than the radius of the sphere.
// To handle these cases, you'll need to pay attention to the problem of catastrophic
// cancellation.  The use of double precision is discouraged.
bool
intersectSphere(
        const Vec3f &rayOrigin, const Vec3f &rayDir, const Vec3f &sphereLoc, float sphereRadius, Vec3f *hitPoint
               ) {
    // Check if sphere ray intersection, and find points of intersection
    // Implemented a naive solution first, then handled numerical stability and
    // then catastrophic cancellation
    Vec3f oc = rayOrigin - sphereLoc;

    // Catastraphic Cancellation happens if radius is very different from distance
    // or distance is very close to radius

    const Vec3f &d = rayDir;
    const Vec3f &f = oc;
    float radiusSquared = sphereRadius * sphereRadius;

    // discriminant = r^2 - (f + b/a * d)^2
    float a = rayDir.Dot(rayDir);
    float b = -f.Dot(rayDir);
    float discriminant = radiusSquared - (f + (b / a) * d).DotSelf();

    // If discriminant is negative, the ray misses the sphere.
    if (discriminant < 0) {
        return false;
    }
    // Else discriminant is positive, ray hits the sphere.
    float c = f.DotSelf() - radiusSquared;
    float q = b + std::copysign(1.0f, b) * std::sqrtf(a * discriminant);

    float t_min = c / q;

    // Find the intersection
    Vec3f p0 = rayOrigin + t_min * rayDir;

    // Copy into hitPoint
    *hitPoint = p0;
    return true;
}

// Problem 3: Sampling a triangle
// Given a list of "triangles" and a uniform random number
// in the range [0, 1) "r", write a function that chooses one
// triangle from the list such that the probability of choosing
// any one triangle is proportional to the area of that triangle
// relative to the total area of all the triangles.
struct Triangle {
    Vec3f p1;
    Vec3f p2;
    Vec3f p3;
};

static float Area(const Triangle &trig) {
    // Cross product / 2 is area of triangle given 3 points
    Vec3f a = trig.p2 - trig.p1;
    Vec3f b = trig.p3 - trig.p1;
    Vec3f cross = a.Cross(b);
    return cross.Length() / 2.0f;
}

static float Remap(float value, float low1, float high1, float low2, float high2) {
    return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

std::size_t
pickTriangle(const std::vector<Triangle> &triangles, const float r) {
    // Not sure what r is, assuming r is the random variable itself.
    std::vector<float> areas; // Initialize a vector to cache the areas of each triangle
    areas.reserve(triangles.size());

    // First we sum all the triangles size while caching the size of each triangle
    // If we use our own loop compiler might be able to do more optimizations
    // Area of triangle
    float totalArea = std::accumulate(triangles.begin(), triangles.end(), 0.f,
                                      [&areas](float accumulate, const Triangle &b) -> float {
                                          float area = Area(b);
                                          areas.emplace_back(area);
                                          return accumulate + area;
                                      });
    // We rescale r to the total area instead of dividing each area by total for greater range
    float new_r = Remap(r, 0.f, 1.f, 0.f, totalArea);

    // We now accumulate until we are greater then uniform random variable r
    float accu = 0;
    std::size_t i;
    for (i = 0; i < areas.size() && accu < r; ++i) {
        accu += areas[i];
    }
    return i;
    // Possible alternative, we can use area^2 instead of area. This would mean avoiding the sqrt operation for finding each area
}


// Problem 4: List of Integers
// Given 2 sorted zero-terminated lists of integers, "old_list" and "new_list",
// write a function which fills in 2 other lists, "added" and "removed", such
// that the added list contains the list of integers in new_list but not
// in old_list, and the removed list contains the list of integers in old_list
// but not in new_list. Assume the inputs are sorted in ascending order and that
// they may contain duplicate entries.  Further, you can assume that "added" and
// "removed" point to memory with adequate space to hold your results.
void
computeAddedAndRemovedElements(
        const int *oldList, const int *newList, int *added, int *removed
                              ) {
    // Added list contains new_list integers that aren't in old_list
    // Removed list contains old_list integers that aren't in new_list

    // There is a possible method to do this with two pointers stepping through
    // each list, and that would be an O(N) algorithm, but it seems quite complex
    // and there are no restrictions on speed and space of algorithm, so I'll use the
    // simplest method I can think of, which is the std::set operations which do the same thing

    // First we find the end iterators for the old and new list for construction of multimaps.
    const int *oldListEnd = oldList;
    const int *newListEnd = newList;
    while (*oldListEnd) {
        ++oldListEnd;
    }
    while (*newListEnd) {
        ++newListEnd;
    }
    // new - old = Add Items
    // old - new = Remove Items
    std::set_difference(newList, newListEnd, oldList, oldListEnd, added);
    std::set_difference(oldList, oldListEnd, newList, newListEnd, removed);

}

// TM and (c) 2017-2020 DreamWorks Animation LLC.  All Rights Reserved.
// Reproduction in whole or in part without prior written permission of a
// duly authorized representative is prohibited.
void Driver() {
    {
        // Test for normalize
        Vec3f a{0, 0, 1};
        Vec3f b{0, 1, 0};
        Vec3f c{1, 0, 0};
        Vec3f d{1, 1, 1};
        Vec3f a_n = normalize(a);
        Vec3f b_n = normalize(b);
        Vec3f c_n = normalize(c);
        Vec3f d_n = normalize(d);

        std::cout << a_n << std::endl;
        std::cout << b_n << std::endl;
        std::cout << c_n << std::endl;
        std::cout << d_n << std::endl;
    }

    // Test for Sphere Intersect (?)

    // Test for picking triangle
    {
        Triangle a{{0, 0, 0},
                   {1, 1, 0},
                   {0, 1, 0}};
        Triangle b{{4, 4, 4},
                   {5, 5, 5},
                   {6, 6, 6}};
    }

    {
        int newArray[] = {1, 1, 2, 3, 4, 54, 64, 65, 65, 65, 65, 69, 0};
        int oldArray[] = {1, 1, 3, 5, 12, 23, 53, 65, 65, 0};
        int added[100] = {};
        int removed[100] = {};
        computeAddedAndRemovedElements(oldArray, newArray, added, removed);
        std::cout << "Finished!" << std::endl;
    }

}