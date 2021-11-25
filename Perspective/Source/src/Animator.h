


#pragma once

#include "Animation.h"
#include "Logger.h"
struct IKBone;

//! Read data from heirachy of bones, recursive through all of them to prepare final data
class Animator {
public:
    Animator(Animation *animation);

    void UpdateAnimation(float dt);

    void PlayAnimation();

    void QueueAnimation(Animation* anim)
    {
        queueAnimation.emplace_back(anim);
        totalAnimTime += anim->GetDuration();
    }

    void ClearQueue()
    {
        queueAnimation.clear();
    }

    

    void CalculateBoneTransform(const AssimpNodeData *node, MyMath::VQS parentTransform);

    const std::vector<MyMath::VQS> &GetFinalBoneMatrices() const;

    void ImGuiDisplay(float dt) const;

    const Animation *GetAnimation() const { return currentAnimation; }


    const std::vector<MyMath::VQS> GatherBoneGlobalTransformation() const;

    std::optional<std::vector<IKBone>> GetIKBones(AssimpNodeData* endAffector);

    void ApplyIK(std::optional<std::vector<IKBone>>& ikBones );

private:
    void GetIKBonesRecur(
        const AssimpNodeData* node, std::vector<MyMath::VQS>& transformations, std::vector<IKBone> & bones
    );

    void ApplyIKRecur(const AssimpNodeData* node, MyMath::VQS parentTransform, const std::vector<IKBone>& ikBones);

private:

    std::vector<MyMath::VQS> modelSpaceTransformations { MAX_BONES, MyMath::VQS()};
    std::vector<MyMath::VQS> globalSpaceTransformations{ MAX_BONES, MyMath::VQS()};
    Animation *currentAnimation = nullptr;
    float currentAnimCurrentTime = 0.f;
    float currentTime;
    float deltaTime;

    std::vector<Animation*> queueAnimation;
    int currentAnimationIndex = 0;
    float totalAnimTime = 0.f;

    mutable LerpMode currentLerpMode = MyMix_L_iS_iL;
};



