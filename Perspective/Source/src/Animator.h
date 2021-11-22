


#pragma once

#include "Animation.h"
#include "Logger.h"
#include <optional>
struct IKBone;

//! Read data from heirachy of bones, recursive through all of them to prepare final data
class Animator {
public:
    Animator(Animation *animation);

    void UpdateAnimation(float dt);

    void PlayAnimation(Animation *pAnimation);

    void CalculateBoneTransform(const AssimpNodeData *node, MyMath::VQS parentTransform);

    const std::vector<MyMath::VQS> &GetFinalBoneMatrices() const;

    void ImGuiDisplay(float dt) const;

    const Animation *GetAnimation() const { return currentAnimation; }


    const std::vector<MyMath::VQS> GatherBoneGlobalTransformation() const;

    std::optional<std::list<IKBone>> GetIKBones(AssimpNodeData* endAffector);

    void ApplyIK(std::optional<std::list<IKBone>>& ikBones );

private:
    void GetIKBonesRecur(
        const AssimpNodeData* node, std::vector<MyMath::VQS>& transformations, std::list<IKBone> & bones
    );

    void ApplyIKRecur(const AssimpNodeData* node, MyMath::VQS parentTransform, const std::list<IKBone>& ikBones);

private:

    std::vector<MyMath::VQS> modelSpaceTransformations { MAX_BONES, MyMath::VQS()};
    std::vector<MyMath::VQS> globalSpaceTransformations{ MAX_BONES, MyMath::VQS()};
    Animation *currentAnimation;
    float currentTime;
    float deltaTime;

    mutable LerpMode currentLerpMode;
};



