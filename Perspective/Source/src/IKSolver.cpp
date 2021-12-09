#include "stdafx.h"
#include "IKSolver.h"

glm::quat fromTo(const glm::vec3& from, const glm::vec3& to) {
	using namespace glm;
	vec3 f = normalize(from);
	vec3 t = normalize(to);

	if (f == t) {
		return quat();
	}
	else if (f == t * -1.0f) {
		vec3 ortho = vec3(1, 0, 0);
		if (fabsf(f.y) < fabsf(f.x)) {
			ortho = vec3(0, 1, 0);
		}
		if (fabsf(f.z) < fabs(f.y) && fabs(f.z) < fabsf(f.x)) {
			ortho = vec3(0, 0, 1);
		}

		vec3 axis = normalize(cross(f, ortho));
		return quat(0, axis.x, axis.y, axis.z);
	}

	vec3 half = normalize(f + t);
	vec3 axis = cross(f, half);

	return quat(
		dot(f, half),
		axis.x,
		axis.y,
		axis.z
	);
}

MyMath::VQS GetGlobalTrans(const std::vector<IKBone>& ikBones, IKBone* targetBone)
{

	MyMath::VQS mat;
	for (auto iter = ikBones.rbegin(); iter != ikBones.rend(); ++iter)
	{
		mat = mat * iter->localTransformation;

		if (targetBone == &(*iter))
		{
			break;
		}
	}

	return MyMath::VQS(mat);
}

bool IKSolver::SolveIK(const glm::vec3& targetPosition, const MyMath::VQS& worldTransform, float t)
{
	if (!ikBones)
		return false;
	auto& IKBones = ikBones.value();

	IKBone& endEffector = IKBones.front();

	glm::vec3 color(1, 0, 0);
	// From (End Effector + 1) to root

	bool retval = SolveInternal(worldTransform, IKBones, endEffector, targetPosition, color, t);

	// Now we have to apply it back to global space transformation
	glm::mat4 mat{ 1.0f };
	bool showDebugInfo = false; // ImGui::TreeNode("Show Bone Debug Info");
	// From root to end effector
	for (auto iter = IKBones.rbegin(); iter != IKBones.rend(); ++iter)
	{
		mat = mat * iter->localTransformation.ToMat4();
		iter->worldTransformation = MyMath::VQS(mat);
		auto pos = worldTransform * iter->worldTransformation;
		dd::box(glm::value_ptr(pos.v), glm::value_ptr(color), 5, 5, 5);


		if (iter != IKBones.rbegin())
		{
			auto nextIter = std::prev(iter);
			auto pos2 = worldTransform * nextIter->worldTransformation;
			dd::arrow(glm::value_ptr(pos2.v), glm::value_ptr(pos.v), glm::value_ptr(color), 10);
		}
		if (showDebugInfo)
		{
			if (iter->animationBone)
				ImGui::Text("Bone: %s", std::string(iter->animationBone->GetBoneName()).c_str());
			else
				ImGui::Text("Bone: NO ANI BONE");
			ImGui::Text("Local Rot: [%f %f %f %f]", pos.q.s, pos.q.v.x, pos.q.v.y, pos.q.v.z);
			ImGui::Text("World Rot: [%f %f %f %f]", iter->localTransformation.q.s, iter->localTransformation.q.v.x, iter->localTransformation.q.v.y, iter->localTransformation.q.v.z);
			ImGui::NewLine();
		}
	}
	if (showDebugInfo)
		ImGui::TreePop();
	auto pos = worldTransform * IKBones.front().worldTransformation;
	color = { 0, 0, 1 };
	dd::box(glm::value_ptr(pos.v), glm::value_ptr(color), 10, 10, 10);


	return retval;
}

bool IKSolver::SolveInternal(const MyMath::VQS& worldTransform, std::vector<IKBone>& IKBones, IKBone& endEffector, const glm::vec3& targetPosition, glm::vec3& color, float t)
{


	for (unsigned int i = 0; i < steps; ++i) {
		// If distance from effector to target is close enough
		glm::vec3 beforeEffectorPos = (worldTransform * GetGlobalTrans(IKBones, &endEffector)).v;
		glm::vec3 yellowCol(0.7, 0.7, 1.0);
		//::cout << "[Start Step]" << i << " ";
		//std::cout << "  Curr End Effector Pos " << glm::to_string(beforeEffectorPos) << " ";
		// std::cout << i << ": Length2 [Target - Effector]: " << glm::length2(targetPosition - beforeEffectorPos) << '\n';
		if (glm::length2(targetPosition - beforeEffectorPos) < threshold2) {
			return true;
		}
		int numOfBones = 0;
		for (int j = 0; j < (IKBones.size() - 2) && numOfBones < bonesToMove; ++j, ++numOfBones)
		{
			for (int k = 1; k < j + 3 && k < IKBones.size(); ++k)
			{
				IKBone& bone = IKBones[k];

				// Get the matrix of the end effector in true world space
				glm::vec3 effectorPos = (worldTransform * GetGlobalTrans(IKBones, &IKBones[0])).v;

				//std::cout << "\t Effector Pos: [" << glm::to_string(effectorPos) << "] ||";

				// Get the joints true world space matrix (bone to world transformation)
				const MyMath::VQS boneToWorldTransform = worldTransform * GetGlobalTrans(IKBones, &IKBones[k]);

				const glm::vec3 position = boneToWorldTransform.v;
				const MyMath::Quaternion rotation = boneToWorldTransform.q;

				//std::cout << "\t Joint Pos: [" << glm::to_string(effectorPos) << "]";

				glm::vec3 toEffector = glm::normalize(effectorPos - position);
				glm::vec3 toGoal = glm::normalize(targetPosition - position);

				if (glm::all(glm::isnan(toEffector)) || glm::all(glm::isnan(toGoal)))
				{
					continue;
				}

				MyMath::Quaternion effectorToGoal{};
				if (glm::length2(toGoal) > 0.00001f) {
					effectorToGoal = MyMath::Quaternion::GetRotationBetween(toEffector, toGoal);

				}

				MyMath::Quaternion localRotate = rotation * effectorToGoal * rotation.Inverse();
				bone.localTransformation.q = localRotate * bone.localTransformation.q;

				effectorPos = (worldTransform * GetGlobalTrans(IKBones, &IKBones[0])).v;
				//std::cout << "\t New Effector Pos: " << glm::to_string(effectorPos) << " ";

				if (glm::length2(targetPosition - effectorPos) < threshold2) {
					return true;
				}
			}
		}
		// If the difference between the two effectors are not far enough, exit
		glm::vec3 afterEffectorPos = (worldTransform * GetGlobalTrans(IKBones, &endEffector)).v;
		if (glm::length2(afterEffectorPos - beforeEffectorPos) < 0.00001f) {
			return false;
		}
		//std::cout << "  New End Effector Pos " << glm::to_string(afterEffectorPos) << " ";
		//std::cout << "[End Step] " ;

	}
	//std::cout << std::endl;
	return false;
}

void IKSolver::ImGuiDispay(const MyMath::VQS& worldTransform)
{
	ImGui::SliderInt("Steps: ", &steps, 0, 1000);
	ImGui::SliderFloat("Threshold", &threshold, 0, 100);
	// ImGui::SliderFloat2("Global Limit", glm::value_ptr(g_limits), -130, 130);
	ImGui::SliderInt("Bones to move", &bonesToMove, 1, 100);

	threshold2 = threshold * threshold;

	if (!ikBones)
		return;

	glm::vec3 color(1, 0, 0);
	glm::vec3 color2(0, 1, 0);
	const auto& IKBones = ikBones.value();
	int i = IKBones.size();
	for (auto iter = IKBones.rbegin(); iter != IKBones.rend(); ++iter, --i)
	{
		auto pos = worldTransform * iter->worldTransformation;
		if (i < bonesToMove)
			dd::box(glm::value_ptr(pos.v), glm::value_ptr(color), 5, 5, 5);
		else
			dd::box(glm::value_ptr(pos.v), glm::value_ptr(color2), 5, 5, 5);

		if (iter != IKBones.rbegin())
		{
			auto nextIter = std::prev(iter);
			auto pos2 = worldTransform * nextIter->worldTransformation;
			dd::arrow(glm::value_ptr(pos2.v), glm::value_ptr(pos.v), glm::value_ptr(color), 10);
		}
	}
}
