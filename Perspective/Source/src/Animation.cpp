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
        static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;
        const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
        const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
        if (ImGui::BeginTable("4ways", 4, flags))
        {
            // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
            ImGui::TableSetupColumn("Translate", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
            ImGui::TableSetupColumn("Quaternion", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
            ImGui::TableSetupColumn("Scale", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
            ImGui::TableHeadersRow();
            rootNode.DisplayImGui(0);
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }

    if(ImGui::TreeNode("Show End Affectors"))
    {
        int i = 0;
        for(int i = 0; i < endAffectors.size(); ++i)
        {
            ImGui::Text("%s", endAffectors[i]->name.c_str());
        }
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
    dest.localTransformation = MyMath::VQS(transformation);
    dest.childrenCount = src->mNumChildren;

    if (src->mNumChildren <= 0)
    {
        endAffectors.emplace_back(&dest);
    }
    for (int i = 0; i < src->mNumChildren; i++) {
        AssimpNodeData* newData = new AssimpNodeData();
        ReadHeirarchyData(*newData, src->mChildren[i]);
        newData->parent = &dest;
        dest.children.emplace_back(newData);
    }
}

void AssimpNodeData::DisplayImGui(int nodeID) const {
    auto &trans = localTransformation.v;
    auto &rotate = localTransformation.q;

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    const bool is_not_end = (childrenCount > 0);

    if (is_not_end) {
        bool open = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
        ImGui::TableNextColumn();
        ImGui::Text("Translate [%f, %f, %f]", trans.x, trans.y, trans.z);
        ImGui::TableNextColumn();
        ImGui::Text("Rotate [%f, %f, %f, %f]", rotate.s, rotate.v.x, rotate.v.y, rotate.v.z);
        ImGui::TableNextColumn();
        ImGui::Text("Scale [%f]", localTransformation.s);

        if (open) {
            for (int child_n = 0; child_n < childrenCount; child_n++)
                children[child_n]->DisplayImGui(nodeID + child_n);
            ImGui::TreePop();
        }
    } else {

        ImGui::TreeNodeEx(name.c_str(),
                          ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen |
                          ImGuiTreeNodeFlags_SpanFullWidth);
        ImGui::TableNextColumn();
        ImGui::Text("Translate [%f, %f, %f]", trans.x, trans.y, trans.z);
        ImGui::TableNextColumn();
        ImGui::Text("Rotate [%f, %f, %f, %f]", rotate.s, rotate.v.x, rotate.v.y, rotate.v.z);
        ImGui::TableNextColumn();
        ImGui::Text("Scale [%f]", localTransformation.s);
    }
}