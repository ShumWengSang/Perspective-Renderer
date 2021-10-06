


#pragma once
#include "Animation.h"
#include "Logger.h"
//! Read data from heirachy of bones, recursive through all of them to prepare final data
class Animator {
	public:
        enum LerpMode
        {
            iVQS,
            Mix
        };

        Animator(Animation* animation);

        void UpdateAnimation(float dt);
             
        void PlayAnimation(Animation* pAnimation);
             
        void CalculateBoneTransform(const AssimpNodeData* node, MyMath::VQS parentTransform);

        const std::vector<MyMath::VQS>& GetFinalBoneMatrices() const;

        void ImGuiDisplay(float dt) const;
        const Animation* GetAnimation() const {return currentAnimation;}


        const std::vector<MyMath::VQS> DrawBones(const MyMath::VQS& mat) const;

private:
    void DrawBoneRecur(const AssimpNodeData* node, const MyMath::VQS& parentMatrix, std::vector<MyMath::VQS>& matrices) const;

private:
    std::vector<MyMath::VQS> finalBoneMatrices;
    Animation* currentAnimation;
    float currentTime;
    float deltaTime;

    LerpMode currentLerpMode;
};



