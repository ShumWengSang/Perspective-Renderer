


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

        void ImGuiDisplay(float dt) const;
        const Animation* GetAnimation() const {return currentAnimation;}


        const std::vector<glm::mat4> DrawBones(const glm::mat4& mat) const;

private:
    void DrawBoneRecur(const AssimpNodeData* node, const glm::mat4& parentMatrix, std::vector<glm::mat4>& matrices) const;

private:
    std::vector<glm::mat4> finalBoneMatrices;
    Animation* currentAnimation;
    float currentTime;
    float deltaTime;
};



