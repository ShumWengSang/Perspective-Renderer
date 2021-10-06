#include "stdafx.h"
#include "Animator.h"
#include "Logger.h"

Animator::Animator(Animation* animation) : finalBoneMatrices(200, MyMath::VQS())
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
        CalculateBoneTransform(&currentAnimation->GetRootNode(), MyMath::VQS());
    }
}

void Animator::PlayAnimation(Animation* pAnimation)
{
    currentAnimation = pAnimation;
    currentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, MyMath::VQS parentTransform)
{
    std::string nodeName = node->name;
    MyMath::VQS nodeTransform = node->transformation;

    Bone* Bone = currentAnimation->FindBone(nodeName);

    if (Bone)
    {
        Bone->Update(currentTime);
        nodeTransform = Bone->GetLocalTransform();
    }

    MyMath::VQS globalTransformation = parentTransform * nodeTransform;

    auto boneInfoMap = currentAnimation->GetBoneIDMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end())
    {
        int index = boneInfoMap[nodeName].id;
        MyMath::VQS offset = boneInfoMap[nodeName].offset;
        finalBoneMatrices[index] = globalTransformation * offset;
    }

    for (int i = 0; i < node->childrenCount; i++)
        CalculateBoneTransform(&node->children[i], globalTransformation);
}

const std::vector<MyMath::VQS>& Animator::GetFinalBoneMatrices() const
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

const std::vector<MyMath::VQS> Animator::DrawBones(const MyMath::VQS& mat) const
{
    std::vector<MyMath::VQS> res;
    if (currentAnimation)
    {
        DrawBoneRecur(&currentAnimation->GetRootNode(), mat, res);
    }
    return res;
}

void Animator::DrawBoneRecur(const AssimpNodeData* node, const MyMath::VQS& parentMatrix, std::vector<MyMath::VQS>& matrices) const
{
    std::string nodeName = node->name;
    MyMath::VQS nodeTransform = node->transformation;

    Bone* Bone = currentAnimation->FindBone(nodeName);

    if (Bone)
    {
        nodeTransform = Bone->GetLocalTransform();
    }
    MyMath::VQS globalTransform = parentMatrix * nodeTransform;
    matrices.emplace_back(globalTransform);
    for (int i = 0; i < node->childrenCount; i++)
        DrawBoneRecur(&node->children[i], globalTransform, matrices);
}
