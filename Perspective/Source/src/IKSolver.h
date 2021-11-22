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

glm::quat fromTo(const glm::vec3& from, const glm::vec3& to);

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

		glm::vec3 color(1, 0, 0);
		// From (End Effector + 1) to root

		bool retval = SolveInternal(worldTransform, IKBones, endEffector, targetPosition, color);

		// Now we have to apply it back to global space transformation
		glm::mat4 mat{1.0f};
		// From root to end effector
		for (auto iter = IKBones.rbegin(); iter != IKBones.rend(); ++iter)
		{
			mat = mat * iter->localTransformation.ToMat4();
			iter->worldTransformation = MyMath::VQS(mat);
			auto pos = worldTransform * iter->worldTransformation;
			dd::box(glm::value_ptr(pos.v), glm::value_ptr(color), 5, 5, 5);
		}
		auto pos = worldTransform * IKBones.front().worldTransformation;
		color = {0, 0, 1};
		dd::box(glm::value_ptr(pos.v), glm::value_ptr(color), 10,10,10);

		return retval;
	}

	bool SolveInternal(const MyMath::VQS& worldTransform, std::list<IKBone>& IKBones, IKBone& endEffector, const glm::vec3& targetPosition, glm::vec3& color)
	{
		for (unsigned int i = 0; i < steps; ++i) {
			glm::vec3 effectorPos = (worldTransform * GetGlobalTrans(IKBones, &endEffector)).v;
			if (glm::length2(targetPosition - effectorPos) < threshold) {
				return true;
			}
			for (auto iter = ++IKBones.begin(); iter != IKBones.end(); ++iter)
			{
				IKBone& bone = *iter;

				// Get the matrix of the end effector in true world space
				effectorPos = (worldTransform * GetGlobalTrans(IKBones, &endEffector)).v;

				// Get the joints true world space matrix (bone to world transformation)
				const MyMath::VQS boneToWorldTransform = worldTransform * GetGlobalTrans(IKBones, &bone);

				const glm::vec3 position = boneToWorldTransform.v;
				const MyMath::Quaternion rotation = boneToWorldTransform.q;

				glm::vec3 toEffector = glm::normalize(effectorPos - position);

				glm::vec3 toGoal = glm::normalize(targetPosition - position);

				if (glm::all(glm::isnan(toEffector)) || glm::all(glm::isnan(toGoal)))
				{
					continue;
				}

				MyMath::Quaternion effectorToGoal;
				if (glm::length2(toGoal) > 0.00001f) {
					effectorToGoal = MyMath::Quaternion::GetRotationBetween(toEffector, toGoal).Norm();
				}

				MyMath::Quaternion localRotate = rotation * effectorToGoal * rotation.Inverse();

				bone.localTransformation.q = localRotate * bone.localTransformation.q;

				effectorPos = (worldTransform * GetGlobalTrans(IKBones, &endEffector)).v;
				if (glm::length2(targetPosition - effectorPos) < threshold) {
					return true;
				}
			}
		}
		return false;
	}

	void SetIKBones(std::optional<std::list<IKBone>>&& bones)
	{
		ikBones = bones;
	}
	void SetSteps(int steps) {this->steps = steps;}
	int GetSteps() const {return steps;}
	std::optional<std::list<IKBone>>& GetIKBones() {return ikBones;}

	void ImGuiDispay()
	{
		ImGui::SliderInt("Steps: ", &steps, 0, 1000);
	}
private:

	

	std::optional<std::list<IKBone>> ikBones;
	float threshold = 2.0f;
	int steps = 1;

};