


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
	Bone(const std::string& name, int ID, const aiNodeAnim* channel);

	void Update(float animationTime, LerpMode lerpMode);

	/* Gets the current index on mKeyPositions to interpolate to based on
the current animation time*/
	int GetPositionIndex(float animationTime);

	/* Gets the current index on mKeyRotations to interpolate to based on the
	current animation time*/
	int GetRotationIndex(float animationTime);

	/* Gets the current index on mKeyScalings to interpolate to based on the
	current animation time */
	int GetScaleIndex(float animationTime);

	MyMath::VQS GetLocalTransform() const { return localTransform; }
	std::string_view GetBoneName() const { return name; }
	int GetBoneID() const { return id; }

private:
	/* Gets normalized value for Lerp & Slerp*/
	float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

	/*figures out which position keys to interpolate b/w and performs the interpolation
and returns the translation matrix*/
	glm::vec3 InterpolatePosition(float animationTime);

	/*figures out which rotations keys to interpolate b/w and performs the interpolation
	and returns the rotation matrix*/
	MyMath::Quaternion InterpolateRotation(float animationTime);

	/*figures out which scaling keys to interpolate b/w and performs the interpolation
	and returns the scale matrix*/
	glm::vec3 InterpolateScaling(float animationTime);

	glm::vec3 InterpolateScalingExpo(float animationTime);




	////////////////// INTERNAL //////////////////////////////
		/*figures out which position keys to interpolate b/w and performs the interpolation
and returns the translation matrix*/
	glm::vec3 InterpolatePositionGLM(float animationTime);

	/*figures out which rotations keys to interpolate b/w and performs the interpolation
	and returns the rotation matrix*/
	glm::quat InterpolateRotationGLM(float animationTime);

	/*figures out which scaling keys to interpolate b/w and performs the interpolation
	and returns the scale matrix*/
	glm::vec3 InterpolateScalingGLM(float animationTime);

	MyMath::VQS InterpolateIncrementalVQM(float animationTime);

	MyMath::Quaternion InterpolateIncrementalSlerp(float animationTime);
private:
	std::vector<KeyPosition> positions;
	std::vector<KeyRotation> rotations;
	std::vector<KeyScale> scales;
	std::vector<KeyFrame> VQSs;

	MyMath::VQS localTransform{};
	MyMath::iVQS incrementalVQSFunc;
	MyMath::iSlerp incrementalSlerp;
	std::string name{};
	int id{};
};



