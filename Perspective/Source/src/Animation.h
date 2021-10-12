


#pragma once

#include "Model.h"
#include "MyMath.h"

struct AssimpNodeData {
    MyMath::VQS transformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;

    void DisplayImGui(int nodeID) const {

        auto& trans = transformation.v;
        auto& rotate = transformation.q;
        ImGui::Text("Translate [%f, %f, %f]", trans.x, trans.y, trans.z);
        ImGui::Text("Rotate [%f, %f, %f, %f]", rotate.s, rotate.v.x, rotate.v.y, rotate.v.z);
        ImGui::Text("Scale [%f]", transformation.s);
        ImGui::NewLine();
        ImGui::Text("Name: ");
        ImGui::SameLine();
        if (childrenCount == 0) {
            ImGui::TextColored({0.8, 0.1, 0.1, 1.0}, "%s", name.c_str());

        } else if (ImGui::TreeNode((void *) (intptr_t) nodeID, "%s", name.c_str(), nodeID)) {

            
            for (int i = 0; i < childrenCount; ++i) {
                children[i].DisplayImGui(nodeID + i);
            }
            ImGui::TreePop();
        }
    }
};

//! Read data from assimp and create a heiracrchy of bones
class Animation {
public:
    Animation() = default;

    Animation(const std::string &animationPath, Model *model);

    Animation(const aiScene *scene, const aiAnimation *animation, Model *model);

    Bone *FindBone(const std::string &name);

    void ImGuiDisplay(float dt) const;

    inline float GetTicksPerSecond() { return ticksPerSecond; }

    inline float GetDuration() { return duration; }

    inline const AssimpNodeData &GetRootNode() { return rootNode; }

    inline const std::unordered_map<std::string, BoneInfo> &GetBoneIDMap() {
        return boneInfoMap;
    }

    inline const std::string &GetName() const { return animName; }

private:
    // Sometimes there are missing bone data that assimp can only get from animation
    void ReadMissingBones(const aiAnimation *animation, Model &model);
    // Read data
    void ReadHeirarchyData(AssimpNodeData &dest, const aiNode *src);


private:
    float duration;
    int ticksPerSecond;
    std::vector<Bone> bones;
    AssimpNodeData rootNode;
    std::unordered_map<std::string, BoneInfo> boneInfoMap;
    std::string animName;
};



