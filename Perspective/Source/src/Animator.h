


#pragma once
#include "Animation.h"
#include "Logger.h"
//! Read data from heirachy of bones, recursive through all of them to prepare final data
class Animator {
	public:
        Animator::Animator(Animation* animation);

        void Animator::UpdateAnimation(float dt);

        void Animator::PlayAnimation(Animation* pAnimation);

        void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

        const std::vector<glm::mat4>& GetFinalBoneMatrices();

private:
    std::vector<glm::mat4> finalBoneMatrices;
    Animation* currentAnimation;
    float currentTime;
    float deltaTime;
};



