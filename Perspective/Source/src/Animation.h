


#pragma once
#include "Model.h"
struct AssimpNodeData
{
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;
};

//! Read data from assimp and create a heiracrchy of bones
class Animation {
    public:
    Animation() = default;
    Animation(const std::string& animationPath, Model* model)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
        assert(scene && scene->mRootNode);

        auto animation = scene->mAnimations[0];
        duration = animation->mDuration;
        ticksPerSecond = animation->mTicksPerSecond;
        ReadHeirarchyData(rootNode, scene->mRootNode);
        ReadMissingBones(animation, *model);
    }
    Animation(const aiScene* scene, const aiAnimation* animation, Model* model)
    {
        duration = animation->mDuration;
        ticksPerSecond = animation->mTicksPerSecond;
        ReadHeirarchyData(rootNode, scene->mRootNode);
        ReadMissingBones(animation, *model);
    }

    Bone* FindBone(const std::string& name)
    {
        auto iter = std::find_if(bones.begin(), bones.end(),
            [&](const Bone& Bone)
            {
                return Bone.GetBoneName() == name;
            }
        );
        if (iter == bones.end()) return nullptr;
        else return &(*iter);
    }


    inline float GetTicksPerSecond() { return ticksPerSecond; }

    inline float GetDuration() { return duration; }

    inline const AssimpNodeData& GetRootNode() { return rootNode; }

    inline const std::unordered_map<std::string, BoneInfo>& GetBoneIDMap()
    {
        return boneInfoMap;
    }

    private:
        // Sometimes there are missing bone data...
        void ReadMissingBones(const aiAnimation* animation, Model& model)
        {
            int size = animation->mNumChannels;

            auto& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
            int& boneCount = model.GetBoneCounter(); //getting the m_BoneCounter from Model class

            //reading channels(bones engaged in an animation and their keyframes)
            for (int i = 0; i < size; i++)
            {
                auto channel = animation->mChannels[i];
                std::string boneName = channel->mNodeName.data;

                if (boneInfoMap.find(boneName) == boneInfoMap.end())
                {
                    Log("Looks like we missed boned ... [%s]", boneName.c_str());
                    boneInfoMap[boneName].id = boneCount;
                    boneCount++;
                }
                bones.emplace_back(Bone(channel->mNodeName.data,
                    boneInfoMap[channel->mNodeName.data].id, channel));
            }

            this->boneInfoMap = boneInfoMap;
        }

        void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src)
        {
            assert(src);

            dest.name = src->mName.data;
            dest.transformation = MyMath::AssimpToMat4(src->mTransformation);
            dest.childrenCount = src->mNumChildren;

            for (int i = 0; i < src->mNumChildren; i++)
            {
                AssimpNodeData newData;
                ReadHeirarchyData(newData, src->mChildren[i]);
                dest.children.emplace_back(newData);
            }
        }


    private:
        float duration;
        int ticksPerSecond;
        std::vector<Bone> bones;
        AssimpNodeData rootNode;
        std::unordered_map<std::string, BoneInfo> boneInfoMap;

};



