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
#include "Rigidbody.h"

namespace {
	Scene scene{};
	std::vector<Model*> model;
	std::vector<Animation*> animation;
	std::list<Rigidbody> rbs;
	std::vector<Entity> boxes;
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

Entity& CS460AssignmentFour::InitEntities(Material* mat, Model* model, float mass, glm::vec3 pos, glm::vec3 scale)
{
	auto& entity = scene.entities.emplace_back(Entity{ model, nullptr,  TransformSystem::getInstance().Create(), nullptr });

	Transform& trans = TransformSystem::getInstance().Get(entity.transformID);
	trans.SetLocalPosition(pos);
	trans.SetLocalScale(scale.x, scale.y, scale.z);
	TransformSystem::getInstance().UpdateMatrices(entity.transformID);

	AABB shape(glm::vec3(pos), trans.scale);
	Collider collider;
	collider.mass = mass;
	collider.SetShape(shape);
	auto& rb = rbs.emplace_back();
	rb.restitution = 0.1f;
	rb.friction = 0.3f;
	rb.AddCollider(collider);
	entity.rb = &rb;
	physicsEnvironment.RegisterRigidbody(&rb);
	return entity;
}



CS460AssignmentFour::~CS460AssignmentFour() {
	for (int i = 0; i < model.size(); ++i)
		delete model[i];
	
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

	PowerPlantMaterial* powerPlantMaterial = new PowerPlantMaterial();
	powerPlantMaterial->ReadMaterialFromFile("Common/PowerPlantFiles/");
	MaterialSystem::getInstance().ManageMaterial(powerPlantMaterial);
	const std::string modelAndAnimationName = "Common/cube2.obj";
	Model* newmodel = new Model(modelAndAnimationName.c_str());
	newmodel->material = powerPlantMaterial;
	// Floor
	{
		InitEntities(powerPlantMaterial, newmodel, 10000, glm::vec3(0), glm::vec3(1000, 10, 1000) );
	}

	// Boxes
	 for(int i = 0; i < 1; ++i)
	 {
		 boxes.emplace_back(InitEntities(powerPlantMaterial, newmodel, 10000,
		 glm::vec3(0, 20 + 50 * (i + 1), 0), glm::vec3(10, 10, 10)));
	 }

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
	scene.directionalLights.emplace_back(sunLight);

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

	for(auto& entity : boxes)
	{
		glm::vec3 gravity {0, -980.f, 0};
		entity.rb->ApplyForce(gravity);
	}
	physicsEnvironment.Update(deltaTime, 4);

	for(auto& entity : scene.entities)
	{
		if(entity.rb)
		{
			Transform& trans = TransformSystem::getInstance().Get(entity.transformID);
			trans.SetLocalPosition(entity.rb->position);
			trans.SetRotationMatrix(glm::toMat4(entity.rb->orientation));
		}
	}

	if(ImGui::Button("Reset Sim"))
	{
		for (int i = 0; i < boxes.size(); ++i)
		{
			boxes[i].rb->position = glm::vec3(0, 20 + 50 * (i + 1),0);
			boxes[i].rb->orientation = glm::quat(1,0,0,0);
		}
	}

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

