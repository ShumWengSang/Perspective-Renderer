#include "stdafx.h"
#include "CS460AssignmentOne.h"
#include "Input.h"
#include "PowerPlantMaterial.h"
#include "MaterialSystem.h"
#include "ModelSystem.h"
#include "GlobalVariables.h"
#include "Model.h"
#include "DebugSystem.h"
#include "Logger.h"
#include "TextureSystem.h"
#include "DirectionalLight.h"
#include "FPSCamera.h"
#include "GBuffer.h"
#include "Scene.h"
#include "LightBuffer.h"
#include "GeometryPass.h"
#include "LightPass.h"
#include "CS350.h"
#include "FinalPass.h"
#include "ForwardRendering.h"
#include "DebugDraw.h"
#include "Model.h"
#include "TransformSystem.h"
#include "Animation.h"
#include "Animator.h"
#include "Phong_AnimatedMaterial.h"
#include <ShaderLocations.h>
#include <implot_internal.h>
#include "CameraUniforms.h"
#include "BezierCurve.h"

namespace {
	Scene scene{};
	Model* model;
	std::vector<Animation*> animation;
	Animator* animator;

	GBuffer gBuffer;
	LightBuffer lightBuffer;

	GeometryPass geometryPass;
	LightPass lightPass;

	FinalPass<AssignmentOne> finalPass;

	ForwardRendering forwardRendering;
	BufferObject<VQS[MAX_BONES]> VQSUniformBlock;
}

CS460AssignmentOne::~CS460AssignmentOne() {
	delete model;
	for (int i = 0; i < animation.size(); ++i)
		delete animation[i];
	delete animator;
}

CS460AssignmentOne::Settings CS460AssignmentOne::Setup() {
	Settings settings{};
	settings.window.size = { 1600, 900 };
	settings.window.resizeable = false;
	settings.context.msaaSamples = 1;

	return settings;
}

void CS460AssignmentOne::Init() {
	// Load power plant material
	PhongAnimatedMaterial* phongAnimated = new PhongAnimatedMaterial();
	phongAnimated->ReadMaterialFromFile("Common/PowerPlantFiles/");
	MaterialSystem::getInstance().ManageMaterial(phongAnimated);

	PowerPlantMaterial* powerPlantMaterial = new PowerPlantMaterial();
	powerPlantMaterial->ReadMaterialFromFile("Common/PowerPlantFiles/");
	MaterialSystem::getInstance().ManageMaterial(powerPlantMaterial);

	// alien
	const std::string modelAndAnimationName = "Common/Animation/alien.fbx";
	model = new Model(modelAndAnimationName.c_str());
	model->material = phongAnimated;
	model->transformID = TransformSystem::getInstance().Create();
	Transform& trans = TransformSystem::getInstance().Get(model->transformID);
	trans.SetLocalPosition(0, 0, -450);
	// trans.SetLocalDirection(0, 180, 0);
	trans.SetLocalScale(0.1);
	TransformSystem::getInstance().UpdateMatrices(model->transformID);

	//Model *cube = new Model("Common/cube.obj");
	//cube->material = powerPlantMaterial;
	//cube->transformID = TransformSystem::getInstance().Create();
	//Transform& cubeTrans = TransformSystem::getInstance().Get(cube->transformID);
	//cubeTrans.SetLocalPosition(0, -50, 0);
	//cubeTrans.SetLocalDirection(0, 0, 0);
	//cubeTrans.SetLocalScale(10000000);
	//TransformSystem::getInstance().UpdateMatrices(cube->transformID);
	//scene.entities.emplace_back(Entity{ cube, nullptr });

	// Animation
	// animation = new Animation("Common/alien.fbx", model);

	Assimp::Importer importer;
	const aiScene* ASSIMPScene = importer.ReadFile(modelAndAnimationName, aiProcess_Triangulate
		| aiProcess_FlipUVs |
		aiProcess_LimitBoneWeights);
	// Get the number of animation
	if (ASSIMPScene) {
		int numOfAnimations = ASSIMPScene->mNumAnimations;
		for (int i = 0; i < numOfAnimations; ++i) {
			animation.emplace_back(new Animation(ASSIMPScene, ASSIMPScene->mAnimations[i], model));
		}
	}
	else {
		LogError("FBX has no animations!");
	}

	Animation* anim = animation.empty() ? nullptr : animation.front();
	animator = new Animator(anim);

	scene.entities.emplace_back(Entity{ model, animator });

	// Load the skybox
	scene.probe.skyCube = TextureSystem::getInstance().LoadCubeMap({
																		   "Common/textures/right.jpg",
																		   "Common/textures/left.jpg",
																		   "Common/textures/bottom.jpg",
																		   "Common/textures/top.jpg",
																		   "Common/textures/front.jpg",
																		   "Common/textures/back.jpg"
		});
	ShaderStruct::DirectionalLight sunLight{};
	sunLight.worldDirection = glm::vec4(-0.2, -1.0, -0.2, 0);
	sunLight.diffuseColor = glm::vec4(1.0, 0.9, 0.9, 0.1);
	sunLight.specularColor = glm::vec4(1.0, 0.9, 0.9, 0.1);
	sunLight.ambientColor = glm::vec4(1.0, 0.9, 0.9, 0.1);
	scene.directionalLights.push_back(sunLight);

	scene.mainCamera = std::make_unique<FpsCamera>();

	scene.mainCamera->LookAt({ 0, 10, -50 }, { 0, 0, 0 });

	VQSUniformBlock.BindBufferBase(BufferObjectType::Uniform, PredefinedUniformBlockBinding(VQSUniformBlock));

	const auto& endAffectors = animator->GetAnimation()->GetEndAffectors();
	auto iter = std::find_if(endAffectors.begin(), endAffectors.end(), [this](const AssimpNodeData* data)
		{
			return data->name == effectorName;
		});
	if(iter != endAffectors.end())
		selectedEffector = *iter;
}

void CS460AssignmentOne::Resize(int width, int height) {
	if (width == 0 || height == 0)
		return;

	scene.mainCamera->Resize(width, height);
	gBuffer.RecreateGpuResources(width, height);
	lightBuffer.RecreateGpuResources(width, height, gBuffer);
}

void CS460AssignmentOne::Draw(const Input& input, float deltaTime, float runningTime) {
	// as a test, generate random data
	static bool once = true;
	if (once) {
		once = false;

		easeInOutVelocity.SetVars(0.2f, 0.8f);

		bezierKnotPoints.emplace_back(glm::vec3(-140, -10, 542));
		bezierKnotPoints.emplace_back(glm::vec3(394, -10, 1596));
		bezierKnotPoints.emplace_back(glm::vec3(843, -10, 1428));

		bezierCurve.AddPoints(bezierKnotPoints);
		generatedPlotPoints.reserve(1000);
	}

	scene.mainCamera->Update(input, deltaTime);
	DebugDrawSystem::getInstance().Update(scene);

	static float cycle = 0.0f;
	cycle += deltaTime * 0.1f;
	cycle = cycle > 1.0f ? 0.0f : cycle;

	const float distance = easeInOutVelocity.CalcDistance(cycle);
	const float velocity = easeInOutVelocity.GetVelocity(cycle);



	// Show the bezier curve and translate model here
	glm::vec3 color{ 1, 0,0 };
	dd::sphere(glm::value_ptr(bezierCurve.Interpolate(distance)), glm::value_ptr(color), 50);
	auto& trans = TransformSystem::getInstance().Get(model->transformID);
	trans.SetLocalPosition(bezierCurve.Interpolate(distance));


	// Calculate orientation with forward method. We will take the average of the next 5 points 
	const int numberOfPoints = 5;
	const float diffBetweenPoints = 0.015f;
	glm::vec3 lastPoint = {};
	glm::vec3 averagePoint = {};
	{
		for (int i = 0; i < numberOfPoints; ++i)
		{
			float t = distance + i * diffBetweenPoints;
			glm::vec3 point;
			if (t > 1.0f)
			{
				// Gradient using last point
				glm::vec3 posA = bezierCurve.Interpolate(0.9999f);
				glm::vec3 posB = bezierCurve.Interpolate(0.9999f - diffBetweenPoints);
				
				point = glm::lerp(posB, posA, t + 0.1f);
			}
			else
			{
				point = bezierCurve.Interpolate(t);
				lastPoint = point;
			}
			glm::vec3 sphereColor = {0,0,1};
			// dd::sphere(glm::value_ptr(point), glm::value_ptr(sphereColor), 5);
			averagePoint += point;

		}
		averagePoint /= numberOfPoints;
	}
	// dd::sphere(glm::value_ptr(averagePoint), glm::value_ptr(color), 25);
	// Average point is where we want to turn our model towards
	  // Find the direction where we need to turn to in world
	  // direction to turn to = Pos - targetPos
	glm::vec3 direction = glm::normalize(averagePoint - trans.position);
	glm::quat q = glm::rotation(glm::vec3(0, 0, 1), direction);
	// trans.SetRotationMatrix(glm::toMat4(q));


	static float animationSpeedModifier = 2.5f;

	// animator->UpdateAnimation(deltaTime* velocity * animationSpeedModifier);
	animator->UpdateAnimation(deltaTime);

	
	{
	static bool debugIK = false;
	ImGui::Checkbox("Debug IK", &debugIK);
	static bool lastSolve = false;
	if(ImGui::Button("Start IK"))
	{ 
		auto optionalBones = animator->GetIKBones(selectedEffector);
		ikSolver.SetIKBones(optionalBones);
		lastSolve = ikSolver.SolveIK(targetPosition, MyMath::VQS(trans.matrix), deltaTime);
	}
	if (lastSolve)
	{
		ImGui::Text("Solved");
	}
	else
	{
		ImGui::Text("Not Solved");
	}
	if (debugIK)
		animator->ApplyIK(ikSolver.GetIKBones());
	}
	if (auto end = ikSolver.GetEndEffactor(); end != nullptr)
	{
		glm::vec3 color (1,1,0);
		dd::box(glm::value_ptr(MyMath::VQS(trans.matrix) * (end->worldTransformation).v),
		glm::value_ptr(color), 10,10,10);
	}

	// animator->UpdateAnimation(deltaTime, this->selectedEffector, this->targetPosition);
    

	const auto& transforms = animator->GetFinalBoneMatrices();
	// Prepare the data for UBO
	for (int i = 0; i < transforms.size(); ++i) {
		if (i >= MAX_BONES)
			throw std::runtime_error("Too many bones!");
		auto& vqs = transforms[i];
		ShaderStruct::VQS mem;
		mem.q = glm::vec4(vqs.q.v.x, vqs.q.v.y, vqs.q.v.z, vqs.q.s);
		mem.s = vqs.s;
		mem.v = vqs.v;
		VQSUniformBlock.memory[i] = mem;
	}
	// Send to UBO
	VQSUniformBlock.UpdateGpuBuffer();

	for (auto& dirLight : scene.directionalLights) {
		dirLight.worldDirection = glm::rotateY(dirLight.worldDirection, deltaTime);
	}
	scene.mainCamera->CommitToGpu();
	geometryPass.Draw(gBuffer, scene);
	lightPass.Draw(lightBuffer, gBuffer, scene);
	finalPass.Draw(gBuffer, lightBuffer, scene);
	forwardRendering.Draw(scene);

	if (ImGui::CollapsingHeader("Inverse Kinematics"))
	{
		ImGui::Text("Target Position");
		glm::vec3 col = { 0,1,0 };
		dd::sphere(glm::value_ptr(this->targetPosition), glm::value_ptr(col), 25);
		ImGui::DragFloat3("Target position", glm::value_ptr(this->targetPosition));

		ikSolver.ImGuiDispay();

		
		ImGui::NewLine();
		//static int selector = -1;
		//int i = 0;
		//if (ImGui::Selectable("NULL", selector == i)) {
		//	selector = i;
		//	selectedEffector = nullptr;
		//}
		//const auto& endAffectors = animator->GetAnimation()->GetEndAffectors();
		//for (int i = 1; i < endAffectors.size() + 1; ++i)
		//{
		//	char buf[32];
		//	sprintf(buf, "%s", endAffectors[i - 1]->name.c_str());
		//	if (ImGui::Selectable(buf, selector == i)) {
		//		selector = i;
		//		selectedEffector = endAffectors[i - 1];
		//	}
		//}
	}

	static int selected = -1;
	if (ImGui::CollapsingHeader("Animation System")) {
		if (ImGui::TreeNode("Select Animation to Play")) {

			for (int n = 0; n < animation.size(); n++) {
				char buf[64];
				sprintf(buf, "#%i: ", n + 1);
				bool selectable = ImGui::Selectable(buf, selected == n);
				ImGui::SameLine();
				ImGui::TextColored({ 0.1, 0.7, 0.1, 1.0 }, "[%s]", animation[n]->GetName().c_str());

				if (selectable) {
					selected = n;
					animator->PlayAnimation(animation[n]);
				}
			}
			ImGui::TreePop();
		}
		animator->ImGuiDisplay(deltaTime);
		if (ImGui::TreeNode("Display current animation heiarchy")) {
			if (selected != -1) {
				animation[selected]->ImGuiDisplay(deltaTime);
			}
			else
				ImGui::TextColored({ 0.7, 0.1, 0.1, 1.0 }, "Select an animation to view this");
			ImGui::TreePop();
		}
	}



	char label[32];

	if (ImGui::CollapsingHeader("Speed and orientation control"))
	{
		// Show imgui info about bezier curve
		ImGui::TextColored({ 0.8, 0.8, 0.1,1.0 }, "t = %f", cycle);

		ImGui::TextColored({ 0.8, 0.1, 0.1,1.0 }, "d = %f", distance);

		ImGui::TextColored({ 0.1,0.1,1.0,1.0 }, "v = %f", velocity);

		easeInOutVelocity.ImguiDisplay();

		glm::vec3 color{ 1, 0,1 };
		dd::sphere(glm::value_ptr(bezierCurve.Interpolate(easeInOutVelocity.CalcDistance(easeInOutVelocity.GetT1()))), glm::value_ptr(color), 50);
		dd::sphere(glm::value_ptr(bezierCurve.Interpolate(easeInOutVelocity.CalcDistance(easeInOutVelocity.GetT2()))), glm::value_ptr(color), 50);

		ImGui::DragFloat("Animation Speed modifier", &animationSpeedModifier, 0.01f);
	}

	if (ImGui::CollapsingHeader("Path")) {
		ImPlotAxisFlags ax_flags = ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks;

		generatedPlotPoints.clear();


		const std::vector<glm::vec3>& controlPoints = bezierCurve.GetGeneratedControlPoints();
		// Render Knots
		auto& knots = bezierCurve.GetPoints();

		for (int i = 0; i < controlPoints.size(); i += 4)
		{
			if (controlPoints.size() % 4 != 0)
				DebugBreak();
			// Generate line from control points
			for (int j = 0; j < 100; ++j) {
				double t = j / 99.0;

				generatedPlotPoints.emplace_back(BezierCurve::Interpolate(t, controlPoints[i], controlPoints[i + 1], controlPoints[i + 2], controlPoints[i + 3]));
			}

		}
		static bool addPoints = false;
		ImGui::Checkbox("Add Points by click?", &addPoints);
		static bool movePoints = false;
		ImGui::Checkbox("Move Points by dragging", &movePoints);

		if (ImGui::TreeNode("Show curve points"))
		{
			for (int i = 0; i < knots.size(); ++i)
			{
				ImGui::Text("Knot %i: [%f, %f, %f]", i, knots[i].x, knots[i].y, knots[i].z);
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Generated Control Points"))
		{
			for (int i = 0; i < controlPoints.size(); ++i)
			{
				ImGui::Text("Control Point %i: [%f, %f, %f]", i, controlPoints[i].x, controlPoints[i].y, controlPoints[i].z);
				if ((i + 1) % 4 == 0)
				{
					ImGui::NewLine();
				}
			}

			ImGui::TreePop();
		}

		if (ImPlot::BeginPlot("Bezier", ImVec2(-1, 0), ImPlotFlags_NoTitle | ImPlotFlags_NoLegend | ImPlotFlags_NoMenus | ImPlotFlags_NoBoxSelect)) {
			ImPlot::SetupAxes(0, 0, ax_flags, ax_flags);
			ImPlot::SetupAxesLimits(-1000, 1000, -1000, 1000);


			// Generate line in the world 
			for (int i = 1; i < generatedPlotPoints.size(); i++)
			{
				glm::vec3 color{ 0, 1, 0 };
				dd::line(glm::value_ptr(generatedPlotPoints[i - 1]), glm::value_ptr(generatedPlotPoints[i]), glm::value_ptr(color));
			}


			for (int i = 0; i < knots.size(); ++i)
			{
				glm::dvec3& line = knots[i];
				if (!movePoints)
				{ 
					ImPlot::DragPoint(i, &line.x, &line.z, ImVec4(0, 0.9f, 0, 1), 4, ImPlotDragToolFlags_NoInputs);
					}
				else
				{ 
					ImPlot::DragPoint(i, &line.x, &line.z, ImVec4(0, 0.9f, 0, 1), 4);
					bezierCurve.Update();
					}
				glm::vec3 color{ 1,0,0 };
				glm::vec3 knotP = knots[i];
				dd::sphere(glm::value_ptr(knotP), glm::value_ptr(color), 25);
			}

			// Set the points
			ImPlot::SetNextLineStyle(ImVec4(0, 0.9f, 0.8, 0.5), 2);
			// ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
			ImPlot::PlotLine("##bez", &generatedPlotPoints[0].x, &generatedPlotPoints[0].z, generatedPlotPoints.size(), 0,
				sizeof(glm::vec3));



			ImPlotPlot& plot = *ImPlot::GetCurrentContext()->CurrentPlot;
			ImPlotAxis& y_axis = plot.YAxis(0);
			ImPlotAxis& x_axis = plot.XAxis(0);
			double u = x_axis.PixelsToPlot(ImGui::GetIO().MousePos.x);
			double v = y_axis.PixelsToPlot(ImGui::GetIO().MousePos.y);


			if (plot.Hovered && input.WasButtonPressed(GLFW_MOUSE_BUTTON_1))
			{
				if (addPoints)
				{
					bezierCurve.AddPoint(glm::vec3(u, -10.0, v));
				}
			}
			else if (plot.Hovered && input.WasButtonPressed(GLFW_MOUSE_BUTTON_2))
			{
				for (int i = 0; i < bezierKnotPoints.size(); ++i)
				{
					std::cout << glm::to_string(bezierKnotPoints[i]) << '\n';
				}
				std::cout << std::endl;
			}
			ImPlot::EndPlot();
		}
	}
}



