

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
#include "Collision.h"
#include "FinalPass.h"
#include "ForwardRendering.h"
#include "DebugDraw.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

namespace {
    Scene scene{};
    Model sphere;

    GBuffer gBuffer;
    LightBuffer lightBuffer;

    GeometryPass geometryPass;
    LightPass lightPass;


    FinalPass<AssignmentOne> finalPass;

    ForwardRendering forwardRendering;

    CollisionMesh factoryCollisionMesh;

}

CS460AssignmentOne::Settings CS460AssignmentOne::Setup() {
    Settings settings{};
    settings.window.size = {1600, 900};
    settings.window.resizeable = false;
    settings.context.msaaSamples = 1;
    return settings;
}

void CS460AssignmentOne::Init() {
    // Load power plant material
    PowerPlantMaterial *powerPlantMaterial = new PowerPlantMaterial();
    powerPlantMaterial->ReadMaterialFromFile("Common/PowerPlantFiles/");
    MaterialSystem::getInstance().ManageMaterial(powerPlantMaterial);

    ModelSystem::getInstance().LoadModel("Common/sphere.obj", [&, powerPlantMaterial](std::vector<Model> models) {
        assert(models.size() == 1);
        Model &model = models[0];
        model.material = powerPlantMaterial;
        scene.models.emplace_back(model);

        Log("Loading of BoundingSphere is a success! \n");
        scene.debugSystem.AddDebugModel(DebugSystem::Face_Normal, model.faceNormal);
        scene.debugSystem.AddDebugModel(DebugSystem::Vertex_Normal, model.vertexNormal);

        auto &LoadedModels = ModelSystem::getInstance().GetAllLoadedModels();
        auto iterator = LoadedModels.find(model.name);
        if (iterator != LoadedModels.end()) {
            const LoadedModel &loadedModel = iterator->second[0];
            factoryCollisionMesh.AddModel(loadedModel);
        } else {
            Log("Mesh %s is not loaded! Weird!", model.name.c_str());
        }
    });

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

    scene.mainCamera->LookAt({0, 0, -20}, {0, 0, 0});
}

void CS460AssignmentOne::Resize(int width, int height) {
    if (width == 0 || height == 0)
        return;

    scene.mainCamera->Resize(width, height);
    gBuffer.RecreateGpuResources(width, height);
    lightBuffer.RecreateGpuResources(width, height, gBuffer);
}

void CS460AssignmentOne::Draw(const Input &input, float deltaTime, float runningTime) {
    scene.mainCamera->Update(input, deltaTime);
    DebugDrawSystem::getInstance().Update(scene);


    for (auto &dirLight : scene.directionalLights) {
        dirLight.worldDirection = glm::rotateY(dirLight.worldDirection, deltaTime);
    }
    scene.mainCamera->CommitToGpu();
    geometryPass.Draw(gBuffer, scene);
    gBuffer.RenderGui("GBuffer");
    lightPass.Draw(lightBuffer, gBuffer, scene);
    LightPass::RenderGui(scene.directionalLights[0], lightBuffer);
    finalPass.Draw(gBuffer, lightBuffer, scene);
    {
        forwardRendering.Draw(scene);
    }
}
