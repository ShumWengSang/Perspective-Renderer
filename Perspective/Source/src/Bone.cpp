
#include "stdafx.h"
#include "Bone.h"

Bone::Bone(const std::string &name, int ID, const aiNodeAnim *channel) : name(name), id(ID)
{
    int numOfPositions = channel->mNumPositionKeys;
    for (int i = 0; i < numOfPositions; ++i)
    {
        aiVector3D aiPosition = channel->mPositionKeys[i].mValue;
        float timeStamp = channel->mPositionKeys[i].mTime;
        KeyPosition data;
        data.position = MyMath::AssimpToGLMVec3(aiPosition);
        data.timeStamp = timeStamp;
        positions.emplace_back(data);
    }

    int numOfRotations = channel->mNumRotationKeys;
    for (int i = 0; i < numOfRotations; ++i)
    {
        aiQuaternion aiRotation = channel->mRotationKeys[i].mValue;
        float timeStamp = channel->mRotationKeys[i].mTime;
        KeyRotation data;
        data.orientation = Quat(aiRotation.w, glm::vec3(aiRotation.x, aiRotation.y, aiRotation.z ));
        data.timeStamp = timeStamp;
        rotations.emplace_back(data);
    }

    int numOfScalings = channel->mNumScalingKeys;
    for (int i = 0; i < numOfScalings; ++i)
    {
        aiVector3D aiScale = channel->mScalingKeys[i].mValue;
        float timeStamp = channel->mScalingKeys[i].mTime;
        KeyScale data;
        data.scale = MyMath::AssimpToGLMVec3(aiScale);
        data.timeStamp = timeStamp;
        scales.emplace_back(data);
    }


}

void Bone::Update(float animationTime, LerpMode lerpMode) {

	switch (lerpMode)
	{
    case LerpMode::MyMix_iL_iS_iL:
    {
        auto translation = InterpolatePosition(animationTime);
        auto rotation = InterpolateIncrementalSlerp(animationTime);
        auto scale = InterpolateScalingExpo(animationTime);
        localTransform = MyMath::VQS(translation, MyMath::Quaternion(rotation), scale.x);
        break;
    }
	case LerpMode::MyMix_L_iSlerp_E:
	{
		auto translation = InterpolatePosition(animationTime);
		auto rotation = InterpolateIncrementalSlerp(animationTime);
		auto scale = InterpolateScalingExpo(animationTime);
		localTransform = MyMath::VQS(translation, MyMath::Quaternion(rotation), scale.x);
		break;
	}
	case LerpMode::MyMix_L_S_E:
	{
		auto translation = InterpolatePosition(animationTime);
		auto rotation = InterpolateRotation(animationTime);
		auto scale = InterpolateScalingExpo(animationTime);
		localTransform = MyMath::VQS(translation, MyMath::Quaternion(rotation), scale.x);
		break;
	}
    case LerpMode::MyMix_L_S_L:
    {
        auto translation = InterpolatePosition(animationTime);
        auto rotation = InterpolateRotation(animationTime);
        auto scale = InterpolateScaling(animationTime);
        localTransform = MyMath::VQS(translation, MyMath::Quaternion(rotation), scale.x);
        break;
    }
	default:
	case LerpMode::GLMMix:
	{
		auto translation = InterpolatePositionGLM(animationTime);
		auto rotation = InterpolateRotationGLM(animationTime);
		auto scale = InterpolateScalingGLM(animationTime);
		localTransform = MyMath::VQS(translation, MyMath::Quaternion(rotation), scale.x);
		break;
	}
	}

}

int Bone::GetPositionIndex(float animationTime) {
    for (int index = 0; index < positions.size() - 1; ++index)
    {
        if (animationTime < positions[index + 1].timeStamp)
            return index;
    }
    return 0;
}

int Bone::GetRotationIndex(float animationTime) {
    for (int index = 0; index < rotations.size() - 1; ++index)
    {
        if (animationTime < rotations[index + 1].timeStamp)
            return index;
    }
    return 0;
}

int Bone::GetScaleIndex(float animationTime) {
    for (int index = 0; index < scales.size() - 1; ++index)
    {
        if (animationTime < scales[index + 1].timeStamp)
            return index;
    }
    return 0;
}

float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
    float currNormalizedLength = animationTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    return currNormalizedLength / framesDiff;
}

glm::vec3 Bone::InterpolatePosition(float animationTime) {
    if (1 == positions.size())
        return positions[0].position;

    const int currIndex = GetPositionIndex(animationTime);
    const auto& currKeyPosition = positions[currIndex];
    const int nextIndex = currIndex + 1;
    const auto& nextKeyPosition = positions[nextIndex];

    float scaleFactor = GetScaleFactor(currKeyPosition.timeStamp,
                                       nextKeyPosition.timeStamp, animationTime);
    glm::vec3 finalPosition = MyMath::Lerp(currKeyPosition.position,
                                       nextKeyPosition.position, scaleFactor);
    return finalPosition;
}

MyMath::Quaternion Bone::InterpolateRotation(float animationTime) {
    if (1 == rotations.size())
    {
        auto rotation = rotations[0].orientation.Norm();
        return rotation;
    }
    const int currIndex = GetRotationIndex(animationTime);
    const auto& currKeyPosition = rotations[currIndex];
    const int nextIndex = currIndex + 1;
    const auto& nextKeyPosition = rotations[nextIndex];


    float scaleFactor = GetScaleFactor(currKeyPosition.timeStamp,
                                       nextKeyPosition.timeStamp, animationTime);
    MyMath::Quaternion quatFinal = MyMath::Slerp(currKeyPosition.orientation,
                                                     nextKeyPosition.orientation, scaleFactor);
    quatFinal = quatFinal.Norm();

    return quatFinal;
}

glm::vec3 Bone::InterpolateScaling(float animationTime) {
    if (1 == scales.size())
        return scales[0].scale;

    const int currIndex = GetScaleIndex(animationTime);
    const auto& currKeyPosition = scales[currIndex];
    const int nextIndex = currIndex + 1;
    const auto& nextKeyPosition = scales[nextIndex];

    float scaleFactor = GetScaleFactor(currKeyPosition.timeStamp,
                                       nextKeyPosition.timeStamp, animationTime);
    glm::vec3 finalScale = MyMath::Lerp(currKeyPosition.scale, nextKeyPosition.scale
            , scaleFactor);
    return finalScale;
}

//////////////////////////////////////////////////////////////////////////
// INTERNAL

glm::vec3 Bone::InterpolatePositionGLM(float animationTime) {
    if (1 == positions.size())
        return positions[0].position;

    const int currIndex = GetPositionIndex(animationTime);
    const auto& currKeyPosition = positions[currIndex];
    const int nextIndex = currIndex + 1;
    const auto& nextKeyPosition = positions[nextIndex];

    float scaleFactor = GetScaleFactor(currKeyPosition.timeStamp,
        nextKeyPosition.timeStamp, animationTime);
    glm::vec3 finalPosition = glm::mix(currKeyPosition.position,
        nextKeyPosition.position, scaleFactor);
    return finalPosition;
}

glm::quat Bone::InterpolateRotationGLM(float animationTime) {
    if (1 == rotations.size())
    {
        auto rotation = rotations[0].orientation.Norm();
        return rotation.ToGLMQuat();
    }
    const int currIndex = GetRotationIndex(animationTime);
    const auto& currKeyPosition = rotations[currIndex];
    const int nextIndex = currIndex + 1;
    const auto& nextKeyPosition = rotations[nextIndex];


    float scaleFactor = GetScaleFactor(currKeyPosition.timeStamp,
        nextKeyPosition.timeStamp, animationTime);
    auto quatFinal = glm::slerp(currKeyPosition.orientation.ToGLMQuat(),
        nextKeyPosition.orientation.ToGLMQuat(), scaleFactor);
    quatFinal = glm::normalize(quatFinal);

    return quatFinal;
}

glm::vec3 Bone::InterpolateScalingGLM(float animationTime) {
    if (1 == scales.size())
        return scales[0].scale;

    const int currIndex = GetScaleIndex(animationTime);
    const auto& currKeyPosition = scales[currIndex];
    const int nextIndex = currIndex + 1;
    const auto& nextKeyPosition = scales[nextIndex];

    float scaleFactor = GetScaleFactor(currKeyPosition.timeStamp,
        nextKeyPosition.timeStamp, animationTime);
    glm::vec3 finalScale = glm::mix(currKeyPosition.scale, nextKeyPosition.scale
        , scaleFactor);
    return finalScale;
}

glm::vec3 Bone::InterpolateScalingExpo(float animationTime) {
    if (1 == scales.size())
        return scales[0].scale;

    const int currIndex = GetScaleIndex(animationTime);
    const auto& currKeyPosition = scales[currIndex];
    const int nextIndex = currIndex + 1;
    const auto& nextKeyPosition = scales[nextIndex];

    float scaleFactor = GetScaleFactor(currKeyPosition.timeStamp,
        nextKeyPosition.timeStamp, animationTime);
    glm::vec3 finalScale = MyMath::ExpoLerp(currKeyPosition.scale, nextKeyPosition.scale
        , scaleFactor);
    return finalScale;
}

MyMath::VQS Bone::InterpolateIncrementalVQM(float animationTime)
{
    if (incrementalVQSFunc.iVQSStep())
    {
        // We have stepped forward
        return incrementalVQSFunc.GetVQSIter();
    }
    else
    {
        // We can't step forward, init again. Find VQS for begin and end
        MyMath::VQS begin{};
        MyMath::VQS end{};

    }
    assert(0);
    return MyMath::VQS();
}

MyMath::Quaternion Bone::InterpolateIncrementalSlerp(float animationTime)
{
    if (incrementalSlerp.iSlerpStep())
    {
        // We have stepped forward
        return incrementalSlerp.GetQuat();
    }
    else
    {
        if (1 == rotations.size())
            return rotations[0].orientation;
        // We can't step forward, init again. Find VQS for begin and end
        const int currIndex = GetRotationIndex(animationTime);
        const auto& currKeyPosition = rotations[currIndex];
        const int nextIndex = currIndex + 1;
        const auto& nextKeyPosition = rotations[nextIndex];
        const float dt = 0.01666666666;
        incrementalSlerp.iSlerpInit(currKeyPosition.orientation, nextKeyPosition.orientation, 
            (nextKeyPosition.timeStamp - currKeyPosition.timeStamp) * dt
            );
        return currKeyPosition.orientation;
    }
    assert(0);
}

