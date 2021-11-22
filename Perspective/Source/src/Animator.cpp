#include "stdafx.h"
#include "Animator.h"
#include "Logger.h"
#include "IKSolver.h"


Animator::Animator(Animation *animation) : modelSpaceTransformations(MAX_BONES, MyMath::VQS()) {
    currentTime = 0.0;
    currentAnimation = animation;
}

void Animator::UpdateAnimation(float dt) {
    deltaTime = dt;
    if (currentAnimation) {
        currentTime += currentAnimation->GetTicksPerSecond() * dt;
        currentTime = fmod(currentTime, currentAnimation->GetDuration());
        CalculateBoneTransform(&currentAnimation->GetRootNode(), MyMath::VQS());
    }
}

void Animator::PlayAnimation(Animation *pAnimation) {
    currentAnimation = pAnimation;
    currentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData *node, MyMath::VQS parentTransform) {
    std::string nodeName = node->name;
    MyMath::VQS nodeTransform = node->localTransformation;

    Bone *Bone = currentAnimation->FindBone(nodeName);

    if (Bone) {
        Bone->Update(currentTime, currentLerpMode);
        nodeTransform = Bone->GetLocalTransform();
    }

    MyMath::VQS globalTransformation = parentTransform * nodeTransform;

    auto boneInfoMap = currentAnimation->GetBoneIDMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
        int index = boneInfoMap[nodeName].id;
        MyMath::VQS offset = boneInfoMap[nodeName].offset;
        modelSpaceTransformations[index] = globalTransformation * offset;
        globalSpaceTransformations[index] = globalTransformation;
    }

    for (int i = 0; i < node->childrenCount; i++)
        CalculateBoneTransform(node->children[i], globalTransformation);
}

const std::vector<MyMath::VQS> &Animator::GetFinalBoneMatrices() const {
    return modelSpaceTransformations;
}

void Animator::ImGuiDisplay(float dt) const {
    if (ImGui::TreeNode("Animator Controller")) {
        if (currentAnimation) {
            ImGui::Text("Currently playing: ");
            ImGui::SameLine();
            ImGui::TextColored({0.0, 0.7, 0.2, 1.0}, "[%s]", currentAnimation->GetName().c_str());
            ImGui::Text("Time: ");
            ImGui::SameLine();
            ImGui::TextColored({0.0, 0.7, 0.2, 1.0}, "%.2f", currentTime);
            ImGui::SameLine();
            ImGui::Text(" \\ ");
            ImGui::SameLine();
            ImGui::TextColored({0.0, 0.7, 0.2, 1.0}, "%.2f", currentAnimation->GetDuration());


            static int selected = -1;
            ImGui::Text("Interpolation Mode ");
            ImGui::Indent();
            ImGui::PushStyleColor(ImGuiCol_Text, {0.2, 0.8, 0.2, 1.0});
            if (ImGui::Selectable("VQS [Lerp_iSlerp_eLerp]", selected == 0)) {
                selected = 0;
                currentLerpMode = LerpMode::MyMix_L_iSlerp_E;
            }
            ImGui::SameLine();
            ImGui::TextColored({0.8, 0.2, 0.1, 1.0}, "60 FPS");
            if (ImGui::Selectable("VQS [iLerp_iSlerp_iLerp]", selected == 4)) {
                selected = 4;
                currentLerpMode = MyMix_iL_iS_iL;
            }
            ImGui::SameLine();
            ImGui::TextColored({0.8, 0.2, 0.1, 1.0}, "60 FPS");
            if (ImGui::Selectable("VQS [Lerp_Slerp_Lerp]", selected == 2)) {
                selected = 2;
                currentLerpMode = LerpMode::MyMix_L_S_L;
            }
            ImGui::SameLine();
            ImGui::TextColored({0.8, 0.2, 0.1, 1.0}, "60 FPS");
            if (ImGui::Selectable("iVQS", selected == 5)) {
                selected = 5;
                currentLerpMode = LerpMode::MyMix_i_VQM;
            }

            if (ImGui::Selectable("VQS [Lerp_Slerp_eLerp]", selected == 3)) {
                selected = 3;
                currentLerpMode = LerpMode::MyMix_L_S_E;
            }

            if (ImGui::Selectable("GLM [Lerp_Slerp_Lerp]", selected == 1)) {
                selected = 1;
                currentLerpMode = LerpMode::GLMMix;
            }
            ImGui::Unindent();
            ImGui::PopStyleColor();
        } else {
            ImGui::TextColored({0.7, 0.1, 0.1, 1.0}, "No animation set currently");
        }
        ImGui::TreePop();
    }
}

const std::vector<MyMath::VQS> Animator::GatherBoneGlobalTransformation() const {
    std::vector<MyMath::VQS> res;
    if (currentAnimation) {
        // GatherBoneGlobalTransformationRecur(&currentAnimation->GetRootNode(), mat, res);
        res = this->globalSpaceTransformations;
    }
    return res;
}

std::optional<std::list<IKBone>> Animator::GetIKBones(AssimpNodeData* endAffector)
{
    if(!endAffector)
        return {};
    
    std::list<IKBone> IKBones;
    std::vector<MyMath::VQS> transformations;
    
    GetIKBonesRecur(endAffector, transformations, IKBones);

    // Calculate global transform of effector
    if(!IKBones.empty())
        IKBones.emplace_front(IKBone{ IKBones.front().worldTransformation, nullptr });
    else
        IKBones.emplace_front(IKBone{ MyMath::VQS(), nullptr});

    return std::make_optional(IKBones);
}

void Animator::ApplyIK(std::optional<std::list<IKBone>>& ikBones)
{
    if(!ikBones)
        return;
    auto& ikList = ikBones.value();
    auto boneInfoMap = currentAnimation->GetBoneIDMap();

    for (IKBone& ikBone : ikList)
    {
        if(ikBone.animationBone)
        { 
            int index = ikBone.animationBone->GetBoneID();
            modelSpaceTransformations[index] = ikBone.worldTransformation * boneInfoMap[std::string(ikBone.animationBone->GetBoneName())].offset;
            globalSpaceTransformations[index] = ikBone.worldTransformation;
        }
    }
}

void Animator::GetIKBonesRecur(const AssimpNodeData* node, std::vector<MyMath::VQS>& transformations, std::list<IKBone>& bones)
{
    if(node == nullptr)
        return;
    std::string nodeName = node->name;


    Bone* Bone = currentAnimation->FindBone(nodeName);
    if(Bone)
    { 
        int index = Bone->GetBoneID();
        auto bonemap = currentAnimation->GetBoneIDMap();
        if(bonemap[node->name].id != index)
            DebugBreak();
        auto nodeTransform = Bone->GetLocalTransform();
        bones.emplace_back(IKBone{ globalSpaceTransformations[index], Bone, nodeTransform, bonemap[nodeName].offset});
    }
    else
    {
        int a= 1;
    }
    GetIKBonesRecur(node->parent, transformations, bones);
}
