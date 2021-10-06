#include "stdafx.h"
#include "Animator.h"
#include "Logger.h"

Animator::Animator(Animation* animation) : finalBoneMatrices(200, glm::mat4(1.0f))
{
    currentTime = 0.0;
    currentAnimation = animation;
}

void Animator::UpdateAnimation(float dt)
{
    deltaTime = dt;
    if (currentAnimation)
    {
        currentTime += currentAnimation->GetTicksPerSecond() * dt;
        currentTime = fmod(currentTime, currentAnimation->GetDuration());
        CalculateBoneTransform(&currentAnimation->GetRootNode(), glm::mat4(1.0f));
    }
}

void Animator::PlayAnimation(Animation* pAnimation)
{
    currentAnimation = pAnimation;
    currentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;

    Bone* Bone = currentAnimation->FindBone(nodeName);

    if (Bone)
    {
        Bone->Update(currentTime);
        nodeTransform = Bone->GetLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    auto boneInfoMap = currentAnimation->GetBoneIDMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end())
    {
        int index = boneInfoMap[nodeName].id;
        glm::mat4 offset = boneInfoMap[nodeName].offset;
        finalBoneMatrices[index] = globalTransformation * offset;
    }

    for (int i = 0; i < node->childrenCount; i++)
        CalculateBoneTransform(&node->children[i], globalTransformation);
}

const std::vector<glm::mat4>& Animator::GetFinalBoneMatrices()
{
    return finalBoneMatrices;
}

void Animator::ImGuiDisplay(float dt) const
{
    if (ImGui::TreeNode("Animator Widget"))
    {
        if (currentAnimation)
        {
            ImGui::Text("Currently playing: ");
            ImGui::SameLine();
            ImGui::TextColored({ 0.0,0.7,0.2,1.0 }, "[%s]", currentAnimation->GetName().c_str());
            ImGui::Text("Time: ");
            ImGui::SameLine();
            ImGui::TextColored({ 0.0,0.7,0.2,1.0 }, "%.2f", currentTime);
            ImGui::SameLine();
            ImGui::Text(" \\ ");
            ImGui::SameLine();
            ImGui::TextColored({ 0.0,0.7,0.2,1.0 }, "%.2f", currentAnimation->GetDuration());
        }
        else
        {
            ImGui::TextColored({ 0.7, 0.1, 0.1, 1.0 }, "No animation set currently");
        }
        ImGui::TreePop();
    }
}
