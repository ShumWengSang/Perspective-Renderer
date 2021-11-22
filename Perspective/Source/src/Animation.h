


#pragma once

#include "Model.h"
#include "MyMath.h"

struct IKBone;

struct AssimpNodeData {
    MyMath::VQS localTransformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData*> children;
    AssimpNodeData* parent = nullptr;
    void DisplayImGui(int nodeID) const;
    MyMath::VQS worldTransformation;
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

    inline const AssimpNodeData &GetRootNode() const { return rootNode; }
    AssimpNodeData& GetRootNode() {return rootNode;}

    inline const std::unordered_map<std::string, BoneInfo> &GetBoneIDMap() {
        return boneInfoMap;
    }

    inline const std::string &GetName() const { return animName; }

    const auto& GetEndAffectors() const
    {
        return endAffectors;
    }

    

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
    std::vector<AssimpNodeData*> endAffectors;
    std::unordered_map<std::string, BoneInfo> boneInfoMap;
    std::string animName;
};



