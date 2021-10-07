#include "stdafx.h"
#include "Animation.h"

Animation::Animation(const std::string &animationPath, Model *model) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(animationPath, aiProcess_Triangulate | aiProcess_OptimizeGraph);
    assert(scene && scene->mRootNode);

    auto animation = scene->mAnimations[0];
    duration = animation->mDuration;
    ticksPerSecond = animation->mTicksPerSecond;
    ReadHeirarchyData(rootNode, scene->mRootNode);
    ReadMissingBones(animation, *model);
    animName = animation->mName.C_Str();
}

Animation::Animation(const aiScene *scene, const aiAnimation *animation, Model *model) {
    duration = animation->mDuration;
    ticksPerSecond = animation->mTicksPerSecond;
    animName = animation->mName.C_Str();
    ReadHeirarchyData(rootNode, scene->mRootNode);
    ReadMissingBones(animation, *model);
}

Bone *Animation::FindBone(const std::string &name) {
    auto iter = std::find_if(bones.begin(), bones.end(),
                             [&](const Bone &Bone) {
                                 return Bone.GetBoneName() == name;
                             }
    );
    if (iter == bones.end()) return nullptr;
    else return &(*iter);
}

// Sometimes there are missing bone data...

void Animation::ImGuiDisplay(float dt) const {
    if (ImGui::TreeNode("Show heiarchy of %s", animName.c_str())) {
        rootNode.DisplayImGui(0);
        ImGui::TreePop();
    }
}

void Animation::ReadMissingBones(const aiAnimation *animation, Model &model) {
    int size = animation->mNumChannels;

    auto &boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
    int &boneCount = model.GetBoneCounter(); //getting the m_BoneCounter from Model class

    //reading channels(bones engaged in an animation and their keyframes)
    for (int i = 0; i < size; i++) {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
            Log("Looks like we missed boned ... [%s]", boneName.c_str());
            boneInfoMap[boneName].id = boneCount;
            boneCount++;
        }
        bones.emplace_back(Bone(channel->mNodeName.data,
                                boneInfoMap[channel->mNodeName.data].id, channel));
    }

    this->boneInfoMap = boneInfoMap;
}

void Animation::ReadHeirarchyData(AssimpNodeData &dest, const aiNode *src) {
    assert(src);

    dest.name = src->mName.data;
    glm::mat4 transformation = MyMath::AssimpToMat4(src->mTransformation);
    dest.transformation = MyMath::VQS(transformation);
    dest.childrenCount = src->mNumChildren;

    for (int i = 0; i < src->mNumChildren; i++) {
        AssimpNodeData newData;
        ReadHeirarchyData(newData, src->mChildren[i]);
        dest.children.emplace_back(newData);
    }
}
