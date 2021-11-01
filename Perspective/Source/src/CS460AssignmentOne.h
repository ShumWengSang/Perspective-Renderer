


#pragma once

#include "App.h"
#include "BezierCurve.h"
#include "EaseInOutVelocity.h"
class CS460AssignmentOne final : public App {
public:
    CS460AssignmentOne() = default;

    virtual ~CS460AssignmentOne();

    Settings Setup() override;

    void Init() override;

    void Resize(int width, int height) override;

    void Draw(Input const &input, float deltaTime, float runningTime) override;

    std::vector<glm::vec3> generatedPlotPoints;
    std::vector<glm::vec3> bezierKnotPoints;

    BezierCurve bezierCurve;
    EaseInOutVelocity<float> easeInOutVelocity;
};



