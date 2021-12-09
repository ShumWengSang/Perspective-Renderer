


#pragma once

#include "App.h"
#include "BezierCurve.h"
#include "EaseInOutVelocity.h"
#include "IKSolver.h"
class Animation;
struct AssimpNodeData;

enum AnimState
{
    Running = 0,
    IK = 1,
    Max
};

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

    // IK variables
    AssimpNodeData* selectedEffector = nullptr;
    Animation* runningAnim = nullptr;
    Animation* idleAnim = nullptr;
    glm::vec3 targetPosition {36, 47, -324};

    IKSolver ikSolver;

    std::string effectorName = "Bone.028_R.019_end";

    std::optional<std::list<IKBone>> optionalBone{};

    AnimState currState = Running;
    AnimState prevState = IK;
};



