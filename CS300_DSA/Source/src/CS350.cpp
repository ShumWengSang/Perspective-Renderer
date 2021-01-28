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
#include "DirectionalLight.h"
#include "TransformSystem.h"
#include "FinalPass.h"

/////////////////////////////////////////////
// Private data
namespace {
    Scene scene{};
    Model testQuad;

    GBuffer gBuffer;
    LightBuffer lightBuffer;

    GeometryPass geometryPass;
    LightPass lightPass;

    FinalPass<AssignmentOne> finalPass;
}

App::Settings AssignmentOne::Setup() {
    Settings settings{};
    settings.window.size = { 1280, 800 };
    settings.window.resizeable = true;
    settings.context.msaaSamples = 1;
    return settings;
}

void AssignmentOne::Init() {
    ModelSystem::getInstance().LoadModel("Common/models/bunny.obj", [&](std::vector<Model> models) {
        assert(models.size() == 1);
        const Model& model = models[0];

        testQuad = model;

        scene.models.emplace_back(model);
        Log("Loading of Bunny is a success!");
    });
    DirectionalLight sunLight {};
    sunLight.worldDirection = glm::vec4(-0.2, -1.0, -0.2, 0);
    sunLight.color = glm::vec4(1.0, 0.9, 0.9, 0.1);
    scene.directionalLights.push_back(sunLight);

    scene.mainCamera = std::make_unique<FpsCamera>();
}

void AssignmentOne::Resize(int width, int height) {
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
    if (testQuad.vao)
    {
        auto& quadTransform = transformSystem.Get(testQuad.transformID);
        {
            quadTransform.position.x = 4.0f * std::cos(runningTime);
            quadTransform.position.z = 3.0f * std::sin(runningTime);
            quadTransform.position.y = 5.0f + 0.25f * std::sin(runningTime * 10.0f);
            quadTransform.orientation = glm::rotate(quadTransform.orientation, deltaTime, { 0, 1, 0 });
        }
        transformSystem.UpdateMatrices(testQuad.transformID);
    }

    geometryPass.Draw(gBuffer, scene);
    lightPass.Draw(lightBuffer, gBuffer, scene);
    finalPass.Draw(lightBuffer, scene);
}

AssignmentOne::AssignmentOne() :transformSystem(TransformSystem::getInstance()){
}
