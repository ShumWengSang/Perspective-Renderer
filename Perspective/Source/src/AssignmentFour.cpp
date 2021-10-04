//
// Created by user on 2/20/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: AssignmentFour.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Perspective
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 2/20/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "AssignmentFour.h"
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
#include "ForwardRendering.h"
#include "GlobalVariables.h"
#include "Shapes.h"
#include "Collision.h"
#include "DebugSystem.h"
#include "DebugDraw.h"
#include "OctTree.h"
#include "../deps/rapidjson/include/rapidjson/stringbuffer.h"
#include "../deps/rapidjson/include/rapidjson/prettywriter.h"
#include "../deps/rapidjson/include/rapidjson/document.h"
#include "BSPTree.h"
#include "Input.h"

/////////////////////////////////////////////
// Private data
namespace {
    Scene scene{};
    // Model sphere;

    GBuffer gBuffer;
    LightBuffer lightBuffer;

    GeometryPass geometryPass;
    LightPass lightPass;


    FinalPass<AssignmentOne> finalPass;

    ForwardRendering forwardRendering;

    CollisionMesh factoryCollisionMesh;
    std::unique_ptr<OctTree> octTree;
    std::unique_ptr<BSPTree> bspTree;
}

App::Settings AssignmentFour::Setup() {
    Settings settings{};
    settings.window.size = { 1600, 900 };
    settings.window.resizeable = false;
    settings.context.msaaSamples = 1;
    return settings;
}

void AssignmentFour::Init() {
    // Load power plant material
    PowerPlantMaterial *powerPlantMaterial = new PowerPlantMaterial();
    powerPlantMaterial->ReadMaterialFromFile("Common/PowerPlantFiles/");
    MaterialSystem::getInstance().ManageMaterial(powerPlantMaterial);

    // Load the sphere to throw into the scene
    


    // Load the skybox
    scene.probe.skyCube = TextureSystem::getInstance().LoadCubeMap({
                                                                           "Common/textures/right.jpg",
                                                                           "Common/textures/left.jpg",
                                                                           "Common/textures/bottom.jpg",
                                                                           "Common/textures/top.jpg",
                                                                           "Common/textures/front.jpg",
                                                                           "Common/textures/back.jpg"
                                                                   });
    ShaderStruct::DirectionalLight sunLight {};
    sunLight.worldDirection = glm::vec4(-0.2, -1.0, -0.2, 0);
    sunLight.diffuseColor = glm::vec4(1.0, 0.9, 0.9, 0.1);
    sunLight.specularColor = glm::vec4(1.0, 0.9, 0.9, 0.1);
    sunLight.ambientColor = glm::vec4(1.0, 0.9, 0.9, 0.1);
    scene.directionalLights.push_back(sunLight);

    scene.mainCamera = std::make_unique<FpsCamera>();

    scene.mainCamera->LookAt({ 0, 0, -20 }, { 0, 0, 0 });

}

void AssignmentFour::Resize(int width, int height) {
    if(width == 0 || height == 0)
        return;

    scene.mainCamera->Resize(width, height);
    gBuffer.RecreateGpuResources(width, height);
    lightBuffer.RecreateGpuResources(width, height, gBuffer);
}

void AssignmentFour::Draw(const Input &input, float deltaTime, float runningTime) {
    glm::vec3 zero(0);
    glm::vec3 one(1);
    dd::cross(glm::value_ptr(zero), 5);
    if (ImGui::CollapsingHeader("Assignment Three Options - "))
    {
        static int TrigEachNode = 1000;
        ImGui::InputInt("Trig Each Node", &TrigEachNode);
        ImGui::NewLine();

        if(ImGui::SmallButton("Generate Oct Tree"))
        {
            octTree.reset(new OctTree(TrigEachNode, {factoryCollisionMesh}));
        }
        ImGui::SameLine();
        if(ImGui::SmallButton("Load Oct Tree from JSON"))
        {
            octTree.reset(OctTreeFromJSON());
            if(octTree)
            {
                Log("Successfully loaded oct tree from json file.");
            }
        }
        ImGui::InputInt("OctTree depth", &OctTreeNode::MAX_DEPTH);

        if(octTree) {
            octTree->ImGuiSettings();
            if(ImGui::SmallButton("Serialize Oct Tree"))
            {
                SerializeOctTree(octTree.get());
            }
        }
        ImGui::NewLine();
        ImGui::NewLine();        if(ImGui::SmallButton("Generate BSP Tree"))
        {
            bspTree.reset(new BSPTree(TrigEachNode, {factoryCollisionMesh}));
        }

        ImGui::SameLine();
        if(ImGui::SmallButton("Load BSP Tree from JSON"))
        {
            bspTree.reset(BSTTreeFromJSON());
            if(bspTree)
            {
                Log("Successfully loaded oct tree from json file.");
            }
        }
        ImGui::SliderFloat("K value", &BSPNode::k_value, 0.0f, 1.0f);
        ImGui::InputInt("BSP depth", &BSPNode::MAX_DEPTH);
        ImGui::InputInt("Random Split Lower Bound", &BSPNode::Good_Enough_Score);
        ImGui::InputInt("Plane Split Sampling Size", &BSPNode::Plane_Sample_Size);

        if(bspTree)
        {
            bspTree->ImGuiSettings();
            if(ImGui::SmallButton("Serialize BSP Tree"))
            {
                SerializeBSTTree(bspTree.get());
            }
        }
    }
    ImGui::NewLine();

    Update(input, deltaTime);
    for (auto& dirLight : scene.directionalLights)
    {
        dirLight.worldDirection = glm::rotateY(dirLight.worldDirection, deltaTime);
    }

    scene.mainCamera->CommitToGpu();
    geometryPass.Draw(gBuffer, scene);
    gBuffer.RenderGui("GBuffer");
    lightPass.Draw(lightBuffer, gBuffer, scene);
    LightPass::RenderGui(scene.directionalLights[0], lightBuffer);
    finalPass.Draw(gBuffer, lightBuffer, scene);

    if(octTree)
        octTree->ForwardRenderTree();
    if(bspTree)
        bspTree->ForwardRenderTree();
    // Now we do forward rendering

    // Copy depth buffer

    // Forward rendering
    {
        forwardRendering.Draw(scene);
    }
}

AssignmentFour::AssignmentFour() : transformSystem(TransformSystem::getInstance()){

}

void AssignmentFour::LoadModelFromTextFile(std::string fileName, Material* mat) {

    static_assert(true, "DEPRECATED");

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


}

void AssignmentFour::AddShape(std::shared_ptr<Shape> shape) {
    Shapes.emplace_back(shape);
}

void AssignmentFour::Update(const Input &input, float dt) {
    scene.mainCamera->Update(input, dt);
    DebugDrawSystem::getInstance().Update(scene);

}

