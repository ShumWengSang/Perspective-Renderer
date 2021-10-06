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
    MyMath::VQS nodeTransform = node->transformation;

    Bone* Bone = currentAnimation->FindBone(nodeName);

    if (Bone)
    {
        Bone->Update(currentTime);
        nodeTransform = Bone->GetLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform.ToMat4();

    auto boneInfoMap = currentAnimation->GetBoneIDMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end())
    {
        int index = boneInfoMap[nodeName].id;
        MyMath::VQS offset = boneInfoMap[nodeName].offset;
        finalBoneMatrices[index] = globalTransformation * offset.ToMat4();
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

const std::vector<glm::mat4> Animator::DrawBones(const glm::mat4& mat) const
{
    std::vector<glm::mat4> res;
    if (currentAnimation)
    {
        DrawBoneRecur(&currentAnimation->GetRootNode(), mat, res);
    }
    return res;
}

void Animator::DrawBoneRecur(const AssimpNodeData* node, const glm::mat4& parentMatrix, std::vector<glm::mat4>& matrices) const
{
    std::string nodeName = node->name;
    MyMath::VQS nodeTransform = node->transformation;

    Bone* Bone = currentAnimation->FindBone(nodeName);

    if (Bone)
    {
        nodeTransform = Bone->GetLocalTransform();
    }
    glm::mat4 globalTransform = parentMatrix * nodeTransform.ToMat4();
    matrices.emplace_back(globalTransform);
    for (int i = 0; i < node->childrenCount; i++)
        DrawBoneRecur(&node->children[i], globalTransform, matrices);
}
