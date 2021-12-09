#include "stdafx.h"
#include "Animator.h"
#include "Logger.h"
#include "IKSolver.h"


Animator::Animator(Animation *animation) : modelSpaceTransformations(MAX_BONES, MyMath::VQS()) {
    currentTime = 0.0;
    queueAnimation.emplace_back(animation);
    PlayAnimation();
}


//void Animator::UpdateAnimation(float dt) {
//    this->deltaTime = dt;
//    if (currentAnimation) {
//        currentAnimCurrentTime += currentAnimation->GetTicksPerSecond() * dt;
//        currentAnimCurrentTime = fmod(currentAnimCurrentTime, currentAnimation->GetDuration());
//        CalculateBoneTransform(&currentAnimation->GetRootNode(), MyMath::VQS());
//    }
//}

void Animator::UpdateAnimation(float dt) {
    this->deltaTime = dt;
    if (currentAnimation) {

        // Update total runtime of animation
        currentTime += dt;
        // Check if current runtime exceeds current animation duration
        if (currentAnimCurrentTime >= currentAnimation->GetDuration())
        {
            // Go to the next animation (circle back)
            currentAnimationIndex = (++currentAnimationIndex) % queueAnimation.size();
            // Set the current animation
            currentAnimation = queueAnimation[currentAnimationIndex];
            // Reset timer
            currentAnimCurrentTime = fmod(currentAnimCurrentTime, currentAnimation->GetDuration());
            // currentAnimCurrentTime = 0.f;
        }
        // Update current animation time
        currentAnimCurrentTime += currentAnimation->GetTicksPerSecond() * dt;


        CalculateBoneTransform(&currentAnimation->GetRootNode(), MyMath::VQS());
        
        currentTime = fmod(currentTime, totalAnimTime);
    }
}

void Animator::PlayAnimation() {

    if(queueAnimation.empty())
        return;
    currentTime = 0.0f;
    totalAnimTime = 0.f;
    for (const auto* anim : queueAnimation)
    {
        totalAnimTime += anim->GetDuration();
    }
    currentAnimationIndex = 0;
    currentAnimCurrentTime = 0;
    currentAnimation = queueAnimation[currentAnimationIndex];
}

void Animator::CalculateBoneTransform(const AssimpNodeData *node, MyMath::VQS parentTransform) {
    std::string nodeName = node->name;
    MyMath::VQS nodeTransform = node->localTransformation;

    Bone *Bone = currentAnimation->FindBone(nodeName);

    if (Bone) {
        Bone->Update(currentAnimCurrentTime, currentLerpMode);
        nodeTransform = Bone->GetLocalTransform();
    }

    MyMath::VQS globalTransformation = parentTransform * nodeTransform;

    const auto& boneInfoMap = currentAnimation->GetBoneIDMap();
    auto iter = boneInfoMap.find(nodeName);
    if (iter != boneInfoMap.end()) {

        int index = iter->second.id;
        MyMath::VQS offset = iter->second.offset;
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
    ImGui::Text("Total Duration: %f", totalAnimTime);
    ImGui::Text("Anim Duration: %f", currentAnimation->GetDuration());
    ImGui::Text("Ticks per second: %f", currentAnimation->GetTicksPerSecond());
    ImGui::Text("Current Anim Time :%f", currentAnimCurrentTime);

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
            if (ImGui::Selectable("GLM [iLerp_iSlerp_eLerp]", selected == 6)) {
                selected = 6;
                currentLerpMode = LerpMode::MyMix_iL_iS_eL;
            }

            if (ImGui::Selectable("GLM [Lerp_iSlerp_iLerp]", selected == 7)) {
                selected = 7;
                currentLerpMode = LerpMode::MyMix_L_iS_iL;
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

std::optional<std::vector<IKBone>> Animator::GetIKBones(AssimpNodeData* endAffector)
{
    if(!endAffector)
        return {};
    
    std::vector<IKBone> IKBones;
    std::vector<MyMath::VQS> transformations;
    

    IKBones.emplace_back(IKBone{ MyMath::VQS(), nullptr });

    GetIKBonesRecur(endAffector, transformations, IKBones);



    return std::make_optional(IKBones);
}

void Animator::ApplyIK(std::optional<std::vector<IKBone>>& ikBones)
{
    if(!ikBones)
        return;
    auto& ikList = ikBones.value();

    ApplyIKRecur(&currentAnimation->GetRootNode(), MyMath::VQS(), ikList);
}

void Animator::ApplyIKRecur(const AssimpNodeData* node, MyMath::VQS parentTransform, const std::vector<IKBone>& ikBones) {
    std::string nodeName = node->name;
    MyMath::VQS nodeTransform = node->localTransformation;

    Bone* Bone = currentAnimation->FindBone(nodeName);

    if (Bone) {
        // Bone->Update(currentTime, currentLerpMode);
        nodeTransform = Bone->GetLocalTransform();

        auto ikBone = std::find_if(ikBones.begin(), ikBones.end(), [Bone](const IKBone& bone)
            {
                return Bone == bone.animationBone;
            });

        if (ikBone != ikBones.end())
        {
            nodeTransform = ikBone->localTransformation;
        }
        else
        {
            int a = 1;
        }
    }

    MyMath::VQS globalTransformation = parentTransform * nodeTransform;

    const auto& boneInfoMap = currentAnimation->GetBoneIDMap();
    auto iter = boneInfoMap.find(nodeName);
    if (iter != boneInfoMap.end()) {
        int index = iter->second.id;
        MyMath::VQS offset = iter->second.offset;
        modelSpaceTransformations[index] = globalTransformation * offset;
        globalSpaceTransformations[index] = globalTransformation;
    }

    for (int i = 0; i < node->childrenCount; i++)
        ApplyIKRecur(node->children[i], globalTransformation, ikBones);
}

void Animator::GetIKBonesRecur(const AssimpNodeData* node, std::vector<MyMath::VQS>& transformations, std::vector<IKBone>& bones)
{
    if(node == nullptr)
        return;
    std::string nodeName = node->name;


    Bone* Bone = currentAnimation->FindBone(nodeName);
    if(Bone)
    { 
        int index = Bone->GetBoneID();
        const auto& bonemap = currentAnimation->GetBoneIDMap();
        auto iter = bonemap.find(nodeName);
        auto nodeTransform = Bone->GetLocalTransform();
        bones.emplace_back(IKBone{ globalSpaceTransformations[index], Bone, nodeTransform});
    }
    else
    {
        int a= 1;
    }
    GetIKBonesRecur(node->parent, transformations, bones);
}
