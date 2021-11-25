


#pragma once

#include "BidirectionalMap.h"
#include <set>

template<class Container>
auto my_equal_range(Container &&container, double target, double epsilon = 0.00001)
-> decltype(container.equal_range(target)) {
    auto lower = container.lower_bound(target - epsilon);
    auto upper = container.upper_bound(target + epsilon);

    if (lower != container.begin()) {
        lower = std::prev(lower);
    }

    return std::make_pair(lower, upper);
}

template<typename A, typename B>
std::pair<B, A> flip_pair(const std::pair<A, B> &p) {
    return std::pair<B, A>(p.second, p.first);
}

template<typename A, typename B>
std::map<B, A> flip_map(const std::map<A, B> &src) {
    std::map<B, A> dst;
    std::transform(src.begin(), src.end(), inserter(dst, dst.begin()),
                   flip_pair<A, B>);
    return dst;
}

class BezierCurve {
public:
    using Point = glm::vec3;
    using Points = std::vector<Point>;
    using ParameterValue = float;
    using ArcLength = float;

    struct CurveSegment {
        CurveSegment() = default;

        CurveSegment(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c, const glm::vec3 &d) :
                p0(a), p1(b), p2(c), p3(d) {}

        glm::vec3 p0;
        glm::vec3 p1;
        glm::vec3 p2;
        glm::vec3 p3;

        // Values that are compared to the entire bezier curve
        float ua = 0;
        float ub = 1;
        float startArcLength = 0;
        float endArcLength = 1;

        std::map<ArcLength, ParameterValue> distanceToParameterValueLookup;

        friend std::ostream &operator<<(std::ostream &os, const CurveSegment &seg);

        // Input: Distance to travel on this curve segment
        glm::vec3 Interpolate(float distance) const;
    };

public:
    static void GetControlPoints(const Points &knots, Points &outFirstControlPoints, Points &outSecondControlPoints);

    static glm::vec3
    Interpolate(double t, const glm::vec3 &P0, const glm::vec3 &P1, const glm::vec3 &P2, const glm::vec3 &P3);

public:
    BezierCurve() = default;

    void AddPoint(Point point);

    void PopPoint();

    void AddPoints(const Points &input_points);

    Point Interpolate(double t);

    void Update();

    std::vector<glm::dvec3> &GetPoints() {
        return points;
    }

    const Points &GetGeneratedControlPoints() const {
        return generatedControlPoints;
    }

private:
    void CaclulateResultantPoints();

    void GenerateDistanceTable();

    inline static float epislon = 0.0005f;

private:
    static glm::vec3 Interpolate(double t, const CurveSegment &segment);

    static std::map<ParameterValue, ArcLength> GenerateTable(const CurveSegment &curveSegment);

private:
    std::vector<glm::dvec3> points;
    Points generatedControlPoints;
    float totalDistance = 0;
    std::vector<CurveSegment> curveSegments;
    std::map<ParameterValue, CurveSegment *> parameterValueToCurveSegmentLookup;

    friend std::ostream &operator<<(std::ostream &os, const BezierCurve::CurveSegment &seg);
};



