#include "stdafx.h"
#include "AABB.h"

AABB::AABB(glm::vec3 const& center, glm::vec3 const& halfExtents) : center(center), halfExtents(halfExtents) {

}

AABB::AABB(glm::vec3 const& lowerBound, glm::vec3 const& upperBound, int nothing) :
    center((lowerBound + upperBound) / 2.0f),
    halfExtents((upperBound - lowerBound) / 2.0f) {
    (void)nothing;
}

std::tuple<glm::vec3, glm::vec3> AABB::GetMinMax() const {
    glm::vec3 max = center + halfExtents;
    glm::vec3 min = center - halfExtents;
    return std::make_tuple(min, max);
}

std::tuple<glm::vec3, glm::vec3> AABB::GetCenterHalfExtents() const {
    return std::make_tuple(center, halfExtents);
}

bool AABB::Collides(const AABB* a, const AABB* b)
{
	return Collides(*a, *b);
}

bool AABB::Collides(const AABB& a, const AABB& b)
{

    const auto [minA, maxA] = a.GetMinMax();
    const auto [minB, maxB] = b.GetMinMax();

    for (unsigned i = 0; i < 3; ++i) {
        if (maxA[i] < minB[i] || maxB[i] < maxA[i])
            return false;
    }
    return true;
}
