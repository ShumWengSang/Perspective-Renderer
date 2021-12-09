#include "stdafx.h"
#include "CS460AssigmmentFour.h"
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
#include "Physics.h"


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

	Physics physicsEnvironment;
}

CS460AssignmentFour::~CS460AssignmentFour() {
	delete model;
	for (int i = 0; i < animation.size(); ++i)
		delete animation[i];
	delete animator;
}

CS460AssignmentFour::Settings CS460AssignmentFour::Setup() {
	Settings settings{};
	settings.window.size = { 1600, 900 };
	settings.window.resizeable = false;
	settings.context.msaaSamples = 1;

	return settings;
}

void CS460AssignmentFour::Init() {
	// Load power plant material
	PhongAnimatedMaterial* phongAnimated = new PhongAnimatedMaterial();
	phongAnimated->ReadMaterialFromFile("Common/PowerPlantFiles/");
	MaterialSystem::getInstance().ManageMaterial(phongAnimated);

	PowerPlantMaterial* powerPlantMaterial = new PowerPlantMaterial();
	powerPlantMaterial->ReadMaterialFromFile("Common/PowerPlantFiles/");
	MaterialSystem::getInstance().ManageMaterial(powerPlantMaterial);

	// box
	const std::string modelAndAnimationName = "Common/cube.obj";
	model = new Model(modelAndAnimationName.c_str());
	model->material = phongAnimated;
	model->transformID = TransformSystem::getInstance().Create();
	Transform& trans = TransformSystem::getInstance().Get(model->transformID);
	trans.SetLocalPosition(0, 0, -450);
	// trans.SetLocalDirection(0, 180, 0);
	trans.SetLocalScale(0.1);
	TransformSystem::getInstance().UpdateMatrices(model->transformID);


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

}

void CS460AssignmentFour::Resize(int width, int height) {
	if (width == 0 || height == 0)
		return;

	scene.mainCamera->Resize(width, height);
	gBuffer.RecreateGpuResources(width, height);
	lightBuffer.RecreateGpuResources(width, height, gBuffer);
}

void CS460AssignmentFour::Draw(const Input& input, float deltaTime, float runningTime) {

	scene.mainCamera->Update(input, deltaTime);
	DebugDrawSystem::getInstance().Update(scene);

	{
		for (auto& dirLight : scene.directionalLights) {
			dirLight.worldDirection = glm::rotateY(dirLight.worldDirection, deltaTime);
		}
		scene.mainCamera->CommitToGpu();
		geometryPass.Draw(gBuffer, scene);
		lightPass.Draw(lightBuffer, gBuffer, scene);
		finalPass.Draw(gBuffer, lightBuffer, scene);
		forwardRendering.Draw(scene);
	}

}

