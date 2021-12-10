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

void CS460AssignmentFour::Reset()
{
	for (int i = 0; i < boxes.size(); ++i)
	{
		boxes[i].rb->Reset();
		boxes[i].rb->position = glm::vec3(0, 20 + 5 * (i + 1), 0);
		boxes[i].rb->orientation = glm::quat(1, 0, 0, 0);
	}
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
		auto& ent = InitEntities(powerPlantMaterial, newmodel, 10000, glm::vec3(0), glm::vec3(10, 10, 10) );
		ent.rb->fixed = true;
		ent.rb->restitution = 0.f;
		ent.rb->friction= 0.5f;
	}
	const float density = 849.f;
	const float volume = 10 * 10 * 10;
	const float mass = density * volume;

	Reset();
	/*{
	auto& ent = boxes.emplace_back(InitEntities(powerPlantMaterial, newmodel, 10,
		glm::vec3(0, 15, 0), glm::vec3(1, 1, 1)));
		ent.rb->restitution = 0.1f;
		ent.rb->friction = 0.3f;
	}*/
	// Boxes
	 for(int i = 0; i < 5; ++i)
	 {
		 auto& ent = boxes.emplace_back(InitEntities(powerPlantMaterial, newmodel, 10,
		 glm::vec3(0, 20 + 5 * (i + 1), 0), glm::vec3(1, 1, 1)));
		 ent.rb->restitution = 0.1f;
		 ent.rb->friction = 0.3f;
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
		entity.rb->useGravity = true;
		/*glm::vec3 gravity {0, -9.8f, 0};
		entity.rb->ApplyForce(gravity);*/
	}
	static int iterations = 4;
	ImGui::DragInt("Iterations for physics", &iterations);
	physicsEnvironment.Update(deltaTime, iterations);

	int i = 0; 
	for(auto& entity : scene.entities)
	{
		if(entity.rb)
		{
			char buffer[100];
			sprintf(buffer, "Position %i: [%f %f %f]", i, entity.rb->position.x, entity.rb->position.y, entity.rb->position.z);
			if(entity.rb->fixed)
			{
				ImGui::Text(buffer);
				// ImGui::DragFloat3(buffer, glm::value_ptr(entity.rb->position), 1, -50, 50);
			}
			else
			{
				ImGui::Text(buffer);
			}
			sprintf(buffer, "Fixed? %i", i);
			ImGui::Checkbox(buffer, &entity.rb->fixed);
			Transform& trans = TransformSystem::getInstance().Get(entity.transformID);
			trans.SetLocalPosition(entity.rb->position);
			trans.SetRotationMatrix(glm::toMat4(entity.rb->orientation));
			++i;
		}
	}

	if(ImGui::Button("Reset Sim"))
	{
		Reset();
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

