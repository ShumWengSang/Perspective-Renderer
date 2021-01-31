//
// Created by user on 1/24/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: CS350.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/24/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "CS350.h"
#include "ModelSystem.h"
#include "Scene.h"
#include "Model.h"
#include "Material.h"
#include "FPSCamera.h"
#include "Logger.h"
#include "GBuffer.h"
#include "LightBuffer.h"
#include "GeometryPass.h"
#include "LightPass.h"
#include "TransformSystem.h"
#include "FinalPass.h"
#include "PowerPlantMaterial.h"
#include "MaterialSystem.h"
#include "DirectionalLight.h"
#include "MyMath.h"
#include "SkyProbe.h"
#include "TextureSystem.h"

/////////////////////////////////////////////
// Private data
namespace {
    Scene scene{};
    Model testQuad;
    Model sphere;

    GBuffer gBuffer;
    LightBuffer lightBuffer;

    GeometryPass geometryPass;
    LightPass lightPass;


    FinalPass<AssignmentOne> finalPass;
}

App::Settings AssignmentOne::Setup() {
    Settings settings{};
    settings.window.size = { 1920, 1080 };
    settings.window.resizeable = false;
    settings.context.msaaSamples = 1;
    return settings;
}

void AssignmentOne::Init() {
    // Load power plant material
    PowerPlantMaterial *powerPlantMaterial = new PowerPlantMaterial();
    powerPlantMaterial->ReadMaterialFromFile("Common/PowerPlantFiles/");
    MaterialSystem::getInstance().ManageMaterial(powerPlantMaterial);

    LoadModelFromTextFile("Common/PowerPlantFiles/Section1.txt", powerPlantMaterial);
    LoadModelFromTextFile("Common/PowerPlantFiles/Section2.txt", powerPlantMaterial);
    LoadModelFromTextFile("Common/PowerPlantFiles/Section3.txt", powerPlantMaterial);
    LoadModelFromTextFile("Common/PowerPlantFiles/Section4.txt", powerPlantMaterial);
    LoadModelFromTextFile("Common/PowerPlantFiles/Section5.txt", powerPlantMaterial);
    LoadModelFromTextFile("Common/PowerPlantFiles/Section6.txt", powerPlantMaterial);
    LoadModelFromTextFile("Common/PowerPlantFiles/Section7.txt", powerPlantMaterial);
    LoadModelFromTextFile("Common/PowerPlantFiles/Section8.txt", powerPlantMaterial);
    LoadModelFromTextFile("Common/PowerPlantFiles/Section9.txt", powerPlantMaterial);
    LoadModelFromTextFile("Common/PowerPlantFiles/Section10.txt", powerPlantMaterial);
    LoadModelFromTextFile("Common/PowerPlantFiles/Section11.txt", powerPlantMaterial);
    LoadModelFromTextFile("Common/PowerPlantFiles/Section12.txt", powerPlantMaterial);
    LoadModelFromTextFile("Common/PowerPlantFiles/Section13.txt", powerPlantMaterial);
    LoadModelFromTextFile("Common/PowerPlantFiles/Section14.txt", powerPlantMaterial);
    LoadModelFromTextFile("Common/PowerPlantFiles/Section15.txt", powerPlantMaterial);
    LoadModelFromTextFile("Common/PowerPlantFiles/Section16.txt", powerPlantMaterial);
    LoadModelFromTextFile("Common/PowerPlantFiles/Section17.txt", powerPlantMaterial);
    LoadModelFromTextFile("Common/PowerPlantFiles/Section18.txt", powerPlantMaterial);
    LoadModelFromTextFile("Common/PowerPlantFiles/Section19.txt", powerPlantMaterial);
    LoadModelFromTextFile("Common/PowerPlantFiles/Section20.txt", powerPlantMaterial);
    LoadModelFromTextFile("Common/PowerPlantFiles/Section21.txt", powerPlantMaterial);

    // Load the skybox
    scene.probe.skyCube = TextureSystem::getInstance().LoadCubeMap({
                                                                 "Common/textures/right.jpg",
                                                                 "Common/textures/left.jpg",
                                                                 "Common/textures/bottom.jpg",
                                                                 "Common/textures/top.jpg",
                                                                 "Common/textures/front.jpg",
                                                                 "Common/textures/back.jpg"
                                                                 });

/*    ModelSystem::getInstance().LoadModel("Common/models/bunny.obj", [&,powerPlantMaterial](std::vector<Model> models) {
        assert(models.size() == 1);
        const Model& model = models[0];

        testQuad = model;
        testQuad.material = powerPlantMaterial;

        scene.models.emplace_back(testQuad);
        Log("Loading of Bunny is a success! \n");
    });

    ModelSystem::getInstance().LoadModel("Common/models/sphere.obj", [&, powerPlantMaterial](std::vector<Model> models) {
        assert(models.size() == 1);
        const Model& model = models[0];
        sphere = model;
        sphere.material = powerPlantMaterial;
        scene.models.emplace_back(sphere);
        Log("Loading of Sphere is a success! \n");
    });*/
    ShaderStruct::DirectionalLight sunLight {};
    sunLight.worldDirection = glm::vec4(-0.2, -1.0, -0.2, 0);
    sunLight.diffuseColor = glm::vec4(1.0, 0.9, 0.9, 0.1);
    sunLight.specularColor = glm::vec4(1.0, 0.9, 0.9, 0.1);
    sunLight.ambientColor = glm::vec4(1.0, 0.9, 0.9, 0.1);
    scene.directionalLights.push_back(sunLight);

    scene.mainCamera = std::make_unique<FpsCamera>();

    scene.mainCamera->LookAt({ 0, 0, -20 }, { 0, 0, 0 });
}

void AssignmentOne::Resize(int width, int height) {
    if(width == 0 || height == 0)
        return;

    scene.mainCamera->Resize(width, height);
    gBuffer.RecreateGpuResources(width, height);
    lightBuffer.RecreateGpuResources(width, height, gBuffer);
}

void AssignmentOne::Draw(const Input &input, float deltaTime, float runningTime) {
    scene.mainCamera->Update(input, deltaTime);
    for (auto& dirLight : scene.directionalLights)
    {
        dirLight.worldDirection = glm::rotateY(dirLight.worldDirection, deltaTime);
    }
/*    if (testQuad.vao)
    {
        auto& quadTransform = transformSystem.Get(testQuad.transformID);
        {
            quadTransform.position.x = 0;
            quadTransform.position.z = 0;
            quadTransform.position.y = 0;
            quadTransform.scale = glm::vec3(30);
            // quadTransform.orientation = glm::rotate(quadTransform.orientation, deltaTime, { 0, 1, 0 });
        }
        transformSystem.UpdateMatrices(testQuad.transformID);
    }
    if(sphere.vao)
    {
        auto& quadTransform = transformSystem.Get(sphere.transformID);
        {
            quadTransform.position.x = 30;
            quadTransform.position.z = 0;
            quadTransform.position.y = 0;
            quadTransform.scale = glm::vec3(10);
            // quadTransform.orientation = glm::rotate(quadTransform.orientation, deltaTime, { 0, 1, 0 });
        }
        transformSystem.UpdateMatrices(sphere.transformID);
    }*/

    // float remappedScale = MyMath::Remap(powerPlantScale, 0.000001f, 0.0005f, 0.1f, 10.0f);

/*    for(int i = 0; i < scene.models.size(); i++)
    {
        Model& model = scene.models[i];
        if(model.vao)
        {
            auto& transform = transformSystem.Get(sphere.transformID);
            {
                transform.SetLocalScale(powerPlantScale);
            }
            transformSystem.UpdateMatrices(sphere.transformID);
        }
    }*/


    scene.mainCamera->CommitToGpu();
    geometryPass.Draw(gBuffer, scene);
    gBuffer.RenderGui("GBuffer");
    lightPass.Draw(lightBuffer, gBuffer, scene);
    LightPass::RenderGui(scene.directionalLights[0], lightBuffer);
    finalPass.Draw(gBuffer, lightBuffer, scene);


}

AssignmentOne::AssignmentOne() :transformSystem(TransformSystem::getInstance()){
}

void AssignmentOne::LoadModelFromTextFile(std::string fileName, Material* mat) {
    std::ifstream ifstream(fileName);
    if(!ifstream.is_open() || ifstream.fail())
    {
        LogError("Reading of file %s failed. Program ending", fileName.c_str());
        return;
    }

    // To emulate the 'directory'
    std::filesystem::path directory (fileName);
    directory.remove_filename();
    std::string direc = directory.string();
    std::string line;

    int numberOfLines = 0;

    while (std::getline(ifstream, line))
        ++numberOfLines;
    Log("Number of lines in text file: %d", numberOfLines);

    // Reset to begining
    ifstream.clear();
    ifstream.seekg(0);

    Log("Currently loading files decribed in %s in another thread", fileName.c_str());

    while (std::getline(ifstream, line))
    {
        // Read each line, each line describes the model.
        // Load each line as a model, add the material to them, and then push back.
        Log("Starting to load model '%s'", (direc + line).c_str());
        ModelSystem::getInstance().LoadModel(direc + line, [&, mat, numberOfLines](std::vector<Model> models) {
            assert(models.size() == 1);
            Model& model = models[0];
            model.material = mat;
            Transform& transform = TransformSystem::getInstance().Get(model.transformID);
            transform.SetLocalScale(this->powerPlantScale);
            TransformSystem::getInstance().UpdateMatrices(model.transformID);
            scene.models.emplace_back(model);
            Log("Loading of %s is a success!", model.name.c_str());

/*            static int count = 0;
            count++;
            if(numberOfLines == count)
            {
                // Call massive function to do stuff
                Log("Now attempting to find middle point of all loaded model vertices!");
                auto& LoadedModels = ModelSystem::getInstance().GetAllLoadedModels();

                // Find the middle point of all the loaded objs
                glm::vec3 minVertex{ INFINITY };
                glm::vec3 maxVertex{ -INFINITY };

                for(auto& pair : LoadedModels)
                {
                    auto& LoadedModels = pair.second;
                    for(auto& loadedModel : LoadedModels)
                    {
                        for(auto& vertex: loadedModel.vertices)
                        {
                            const Vertex &v = vertex;
                            {
                                minVertex.x = fmin(minVertex.x, v.position.x);
                                minVertex.y = fmin(minVertex.y, v.position.y);
                                minVertex.z = fmin(minVertex.z, v.position.z);

                                maxVertex.x = fmax(maxVertex.x, v.position.x);
                                maxVertex.y = fmax(maxVertex.y, v.position.y);
                                maxVertex.z = fmax(maxVertex.z, v.position.z);
                            }
                        }
                    }
                }
                Log("Vertice data collected.");
                glm::vec3 middle = (minVertex + maxVertex);
                middle /= 2;
                // Log("Now moving camera to middle! [%f, %f, %f]", middle.x, middle.y, middle.z);
                // Now we set camera here
                // scene.mainCamera->LookAt(middle, glm::vec3(0));
            }*/
        });
    }
}
