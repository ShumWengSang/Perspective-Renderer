


#pragma once
#include "Model.h"
struct AssimpNodeData
{
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;

    void DisplayImGui(int nodeID) const
    {
        if (childrenCount == 0)
        {
            ImGui::TextColored({0.8, 0.1,0.1,1.0}, "%s", name.c_str());
        }
        else if (ImGui::TreeNode((void*)(intptr_t)nodeID, "%s", name.c_str(), nodeID))
        {
            for (int i = 0; i < childrenCount; ++i)
            {
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
    Animation(const std::string& animationPath, Model* model);
    Animation(const aiScene* scene, const aiAnimation* animation, Model* model);

    Bone* FindBone(const std::string& name);


    inline float GetTicksPerSecond() { return ticksPerSecond; }

    inline float GetDuration() { return duration; }

    inline const AssimpNodeData& GetRootNode() { return rootNode; }

    inline const std::unordered_map<std::string, BoneInfo>& GetBoneIDMap()
    {
        return boneInfoMap;
    }
    const std::string& GetName() const {return animName;}
    void ImGuiDisplay(float dt) const
    {
        if (ImGui::TreeNode("Show heiarchy of %s", animName.c_str()))
        {
            rootNode.DisplayImGui(0);
            ImGui::TreePop();
        }
    }
    
    private:
        // Sometimes there are missing bone data...
        void ReadMissingBones(const aiAnimation* animation, Model& model);

        void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);


    private:
        float duration;
        int ticksPerSecond;
        std::vector<Bone> bones;
        AssimpNodeData rootNode;
        std::unordered_map<std::string, BoneInfo> boneInfoMap;
        std::string animName;
};



