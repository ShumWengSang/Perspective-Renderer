#pragma once
#include "MyMath.h"
#include <optional>
#include "Bone.h"

struct IKBone
{
	MyMath::VQS worldTransformation = {};
	const Bone* animationBone = nullptr;
	MyMath::VQS localTransformation = {};
	MyMath::VQS offset = {};
};

inline MyMath::VQS GetGlobalTrans(const std::list<IKBone>& ikBones, IKBone* targetBone)
{
	// glm::mat4 mat(1.0f);
	//MyMath::VQS mat;
	//for (auto iter = ikBones.rbegin(); iter != ikBones.rend(); ++iter)
	//{
	//	mat = mat * iter->localTransformation;

	//	if (targetBone == &(*iter))
	//	{
	//		break;
	//	}
	//}

	glm::mat4 mat(1.0f);
	// From root to end effector
	for (auto iter = ikBones.rbegin(); iter != ikBones.rend(); ++iter)
	{
		mat = mat * iter->localTransformation.ToMat4();

		if (targetBone == &(*iter))
		{
			break;
		}
	}

	return MyMath::VQS(mat);
}

class IKSolver
{
public:
	bool SolveIK(const glm::vec3& targetPosition, const MyMath::VQS& worldTransform)
	{
		if(!ikBones)
			return false;
		auto& IKBones = ikBones.value();

		IKBone& endEffector = IKBones.front();

		ImGui::Text("Target Position: [%f %f %f]", targetPosition.x, targetPosition.y, targetPosition.z);

		glm::vec3 color (1,0,0);
		// From End Effector to root
		for (auto iter = IKBones.begin(); iter != IKBones.end(); ++iter)
		{
			IKBone& bone = *iter;

			// Get the matrix of the end effector in true world space
			const MyMath::VQS effectorWorldTransform = worldTransform * GetGlobalTrans(IKBones, &endEffector);
			// Get the joints true world space matrix
			const MyMath::VQS jointTransform = worldTransform * GetGlobalTrans(IKBones, &bone);
			// Now we transform the end effector's position to the joints local space
			const glm::vec3 effectorPos = jointTransform.Inverse().ToMat4() * glm::vec4(effectorWorldTransform.v, 1);
			// Get the position of the joint in joint space (since joint is in joint space its 0,0,0)
			const glm::vec3 jointPos = {0,0,0};
			// Transform the targetposition from true world space to joint space
			const glm::vec3 targetPosition_JointSpace = jointTransform.Inverse().ToMat4() * glm::vec4(targetPosition, 1);

			// Distance too short
			if (glm::distance(effectorPos, targetPosition_JointSpace) <= threshold)
			{
				std::cout << "Distance too short" << std::endl;
				return false;
			}

			// Get the vector from joint to effector
			const glm::vec3 directionToEffector = glm::normalize(effectorPos - jointPos);
			// Check for parallals
			if(glm::all(glm::isnan(directionToEffector)))
				continue;

			// Get the vector from joint to target
			const glm::vec3 directionToGoal = glm::normalize(targetPosition_JointSpace - jointPos);
			// rotate joint so that its closer 
			const MyMath::Quaternion rotation = MyMath::Quaternion::GetRotationBetween(directionToEffector, directionToGoal);
			// Slerp the rotation (todo)
			const auto rot = MyMath::Slerp(jointTransform.q, rotation, 1.0f).Norm();

			// Find the transformation matrix that converts from joint space to local space
			const auto jointToLocal = bone.offset.Inverse();
			// Convert the rotation from joint space to local space
			const auto localSpaceRot = jointToLocal * MyMath::VQS({}, rot, 1.0);
			// Apply to local space rotation
			bone.localTransformation = MyMath::VQS(localSpaceRot) * bone.localTransformation;
		}

		// Now we have to apply it back to global space transformation
		MyMath::VQS mat{};
		// From root to end effector
		for (auto iter = IKBones.rbegin(); iter != IKBones.rend(); ++iter)
		{
			mat = mat * iter->localTransformation;
			iter->worldTransformation = mat;
		}
		auto pos = worldTransform * IKBones.front().worldTransformation;
		dd::box(glm::value_ptr(pos.v), glm::value_ptr(color), 10,10,10);

		return true;
	}

	void SetIKBones(std::optional<std::list<IKBone>>&& bones)
	{
		ikBones = bones;
	}

	std::optional<std::list<IKBone>>& GetIKBones() {return ikBones;}
private:
	std::optional<std::list<IKBone>> ikBones;
	float threshold = 2.0f;

};