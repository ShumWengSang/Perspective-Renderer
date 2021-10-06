


#pragma once
#include "Quaternions.h"
#include "MyMath.h"
#include <Logger.h>
using Quat = MyMath::Quaternion;
struct KeyPosition
{
	glm::vec3 position;
	float timeStamp;
};

struct KeyRotation
{
	Quat orientation;
	float timeStamp;
};

struct KeyScale
{
	glm::vec3 scale;
	float timeStamp;
};

struct KeyFrame
{
	MyMath::VQS vqs;
	float timeStamp;
};


struct BoneInfo
{
	/*id is index in finalBoneMatrices*/
	int id;

	/*offset matrix transforms vertex from model space to bone space*/
	MyMath::VQS offset;

};


class Bone {
public:
	//Read keyframe from aiNodeAnim
	Bone(const std::string& name, int ID, const aiNodeAnim* channel) : name(name), id(ID)
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

		if (numOfPositions != numOfRotations != numOfScalings)
		{
			static_assert(true, "Woah not expected at all!");
		}
	}

	void Update(float animationTime)
	{
		auto translation = InterpolatePosition(animationTime);
		auto rotation = InterpolateRotation(animationTime);
		auto scale = InterpolateScaling(animationTime);
		localTransform = MyMath::VQS(translation, rotation, scale.x);
	}

	/* Gets the current index on mKeyPositions to interpolate to based on
the current animation time*/
	int GetPositionIndex(float animationTime)
	{
		for (int index = 0; index < positions.size() - 1; ++index)
		{
			if (animationTime < positions[index + 1].timeStamp)
				return index;
		}
		assert(0);
	}

	/* Gets the current index on mKeyRotations to interpolate to based on the
	current animation time*/
	int GetRotationIndex(float animationTime)
	{
		for (int index = 0; index < rotations.size() - 1; ++index)
		{
			if (animationTime < rotations[index + 1].timeStamp)
				return index;
		}
		assert(0);
	}

	/* Gets the current index on mKeyScalings to interpolate to based on the
	current animation time */
	int GetScaleIndex(float animationTime)
	{
		for (int index = 0; index < scales.size() - 1; ++index)
		{
			if (animationTime < scales[index + 1].timeStamp)
				return index;
		}
		assert(0);
	}

	MyMath::VQS GetLocalTransform() const { return localTransform; }
	std::string_view GetBoneName() const { return name; }
	int GetBoneID() const { return id; }

	private:
		/* Gets normalized value for Lerp & Slerp*/
		float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
		{
			float currNormalizedLength = animationTime - lastTimeStamp;
			float framesDiff = nextTimeStamp - lastTimeStamp;
			return currNormalizedLength / framesDiff;
		}

		/*figures out which position keys to interpolate b/w and performs the interpolation
and returns the translation matrix*/
		glm::vec3 InterpolatePosition(float animationTime)
		{
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

		/*figures out which rotations keys to interpolate b/w and performs the interpolation
		and returns the rotation matrix*/
		MyMath::Quaternion InterpolateRotation(float animationTime)
		{
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
			glm::quat finalRotation = glm::slerp(currKeyPosition.orientation.ToGLMQuat(),
				nextKeyPosition.orientation.ToGLMQuat(), scaleFactor);
			finalRotation = glm::normalize(finalRotation);
			return MyMath::Quaternion(finalRotation);
		}

		/*figures out which scaling keys to interpolate b/w and performs the interpolation
		and returns the scale matrix*/
		glm::vec3 Bone::InterpolateScaling(float animationTime)
		{
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



private:
	std::vector<KeyPosition> positions;
	std::vector<KeyRotation> rotations;
	std::vector<KeyScale> scales;
	std::vector<KeyFrame> VQSs;

	MyMath::VQS localTransform{};
	std::string name{};
	int id{};
};



