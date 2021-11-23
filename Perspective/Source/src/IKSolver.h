#pragma once
#include "MyMath.h"
#include <optional>
#include "Bone.h"

struct IKBone
{
	MyMath::VQS worldTransformation = {};
	const Bone* animationBone = nullptr;
	MyMath::VQS localTransformation = {};
	
	glm::vec2 limits;
	glm::vec3 axis;
};

glm::quat fromTo(const glm::vec3& from, const glm::vec3& to);

MyMath::VQS GetGlobalTrans(const std::vector<IKBone>& ikBones, IKBone* targetBone);

class IKSolver
{
public:
	bool SolveIK(const glm::vec3& targetPosition, const MyMath::VQS& worldTransform, float t);

	bool SolveInternal(const MyMath::VQS& worldTransform, std::vector<IKBone>& IKBones, IKBone& endEffector, const glm::vec3& targetPosition, glm::vec3& color, float t);

	void SetIKBones(std::optional<std::vector<IKBone>>& bones)
	{
		ikBones = bones;
	}
	void SetSteps(int steps) {this->steps = steps;}
	int GetSteps() const {return steps;}
	std::optional<std::vector<IKBone>>& GetIKBones() {return ikBones;}

	const IKBone* GetEndEffactor() const
	{
		if (ikBones)
		{
			return &ikBones.value().front();
		}
		return nullptr;
	}

	void ImGuiDispay()
	{
		ImGui::SliderInt("Steps: ", &steps, 0, 1000);
		ImGui::SliderFloat("Threshold", &threshold, 0, 100);
		// ImGui::SliderFloat2("Global Limit", glm::value_ptr(g_limits), -130, 130);
		ImGui::SliderInt("Bones to move", &bonesToMove, 1, 100);

		threshold2 = threshold * threshold;
	}
	
private:
	int bonesToMove = 5;
	glm::vec2 g_limits{};

	std::optional<std::vector<IKBone>> ikBones;
	float threshold = 63.0f;
	float threshold2 = 4.0f;
	int steps = 100;

};