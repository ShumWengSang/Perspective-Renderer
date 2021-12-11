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
#include "Joint.h"

namespace {
	Scene scene{};
	std::vector<Model*> model;
	std::vector<Animation*> animation;
	std::vector<Rigidbody> rbs;
	std::vector<Joint> joints;
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

Entity& CS460AssignmentFour::InitEntities(Model* model, float mass, glm::vec3 pos, glm::vec3 scale)
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
	scene.entities.clear();
	rbs.clear();
	boxes.clear();
	Entity floor;
	// Floor
	{
		floor = InitEntities(cubeModel, 10000, glm::vec3(0), glm::vec3(10, 2, 10));
		floor.rb->fixed = true;
		floor.rb->restitution = 0.f;
		floor.rb->friction = 0.5f;
	}

	switch(mode)
	{
	default:
	case 0:
		for (int i = 0; i < 5; ++i)
		{
			auto& ent = boxes.emplace_back(InitEntities(cubeModel, 10,
				glm::vec3(0, 20 + 5 * (i + 1), 0), glm::vec3(1, 1, 1)));
			ent.rb->restitution = 0.1f;
			ent.rb->friction = 0.3f;
		}
		break;
	case 1:
		for (int i = 0; i < 10; ++i)
		{
			auto& ent = boxes.emplace_back(InitEntities(cubeModel, 10,
				glm::vec3(0, 20 + 50 * (i + 1), 0), glm::vec3(1, 1, 1)));
			ent.rb->restitution = 0.1f;
			ent.rb->friction = 0.3f;
		}
		break;
	case 2:
		auto& ent = boxes.emplace_back(InitEntities(cubeModel, 10, glm::vec3(9, 11, 0), glm::vec3(1, 1, 1)));
		ent.rb->friction = 0.2f;
		
		Joint& joint = joints.emplace_back();
		joint->set(floor.rb, ent.rb, glm::vec3(0.0f, 11.0f, 0));
		physicsEnvironment.RegisterJoint(&joint);
		break;
	case 3:
		const int numPlanks = 15;
		float mass = 50.0f;

		for (int i = 0; i < numPlanks; ++i)
		{
			auto& ent = boxes.emplace_back(InitEntities(cubeModel, mass, glm::vec3(-8.5f + 1.25f * i, 5.0f, 0), glm::vec3(1, 1, 1)));
			ent.rb->friction = 0.2f;
		}

		// Tuning
		float frequencyHz = 2.0f;
		float dampingRatio = 0.7f;

		// frequency in radians
		float omega = 2.0f * 3.1415 * frequencyHz;

		// damping coefficient
		float d = 2.0f * mass * dampingRatio * omega;

		// spring stifness
		float k = mass * omega * omega;

		// magic formulas
		constexpr float timeStep = (1 / 60.f);
		float softness = 1.0f / (d + timeStep * k);
		float biasFactor = timeStep * k / (d + timeStep * k);

		for (int i = 0; i < numPlanks; ++i)
		{
			Joint& joint = joints.emplace_back();
			joint.Set(boxes[i].rb, boxes[i + 1].rb, glm::vec3(-9.125f + 1.25f * i, 5.0f, 0.0f));
			joint.softness = softness;
			joint.biasFactor = biasFactor;

			physicsEnvironment.RegisterJoint(&j);
		}

		j->Set(bodies + numPlanks, bodies, Vec2(-9.125f + 1.25f * numPlanks, 5.0f));
		j->softness = softness;
		j->biasFactor = biasFactor;
		world.Add(j);

	}
}


void CS460AssignmentFour::Init() {
	rbs.reserve(2000);

	PowerPlantMaterial* ppMat = new PowerPlantMaterial();
	ppMat->ReadMaterialFromFile("Common/PowerPlantFiles/");
	MaterialSystem::getInstance().ManageMaterial(ppMat);
	powerPlantMaterial = ppMat;

	const std::string modelAndAnimationName = "Common/cube2.obj";
	cubeModel = new Model(modelAndAnimationName.c_str());
	cubeModel->material = powerPlantMaterial;


	 Reset();

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

	for(auto& entity : scene.entities)
	{
		if(entity.rb)
		{
			Transform& trans = TransformSystem::getInstance().Get(entity.transformID);
			trans.SetLocalPosition(entity.rb->position);
			trans.SetRotationMatrix(glm::toMat4(entity.rb->orientation));
		}
	}

	static int forceMult = 100000;
	ImGui::SliderInt("Firing Force", &forceMult, 1, 1000000);
	if(input.WasKeyReleased(GLFW_KEY_SPACE))
	{
		auto& ent = boxes.emplace_back(InitEntities(cubeModel, 10,
			glm::vec3(scene.mainCamera->GetPosition()), glm::vec3(1, 1, 1)));
		ent.rb->ApplyForce(scene.mainCamera->GetForward() * forceMult, scene.mainCamera->GetPosition());
	}


	char buffer[100];
	for (int i = 0; i < 5; ++i)
	{
		sprintf(buffer, "Start scenarion %i", i);
		if (ImGui::Button(buffer))
		{
			mode = i;
			Reset();
		}
	}

	// static int selected = -1;
	if (ImGui::CollapsingHeader("Choose Integration Mode"))
	{
		for (int n = 0; n < PhysicsIntegrateMode::Count; n++)
		{
			char buf[64];
			sprintf(buf, "%s", PhysicsIntergrateModeNames[n]);
			if (ImGui::Selectable(buf, Physics::mode == n))
				Physics::mode = static_cast<PhysicsIntegrateMode>(n);
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

