//
// Created by user on 2/20/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: AssignmentThree.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Perspective
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 2/20/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "AssignmentThree.h"
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

    ForwardRendering forwardRendering;

    CollisionMesh factoryCollisionMesh;
    std::unique_ptr<OctTree> octTree;
    std::unique_ptr<BSPTree> bspTree;
}

App::Settings AssignmentThree::Setup() {
    Settings settings{};
    settings.window.size = {1600, 900};
    settings.window.resizeable = false;
    settings.context.msaaSamples = 1;
    return settings;
}

void AssignmentThree::Init() {
    // Load power plant material
    PowerPlantMaterial *powerPlantMaterial = new PowerPlantMaterial();
    powerPlantMaterial->ReadMaterialFromFile("Common/PowerPlantFiles/");
    MaterialSystem::getInstance().ManageMaterial(powerPlantMaterial);

    // Load Oct Tree for Section 10
    octTree.reset(OctTreeFromJSON());
    bspTree.reset(BSTTreeFromJSON());

    if (Global::loadFile.empty()) {
        //LoadModelFromTextFile("Common/PowerPlantFiles/Section1.txt", powerPlantMaterial);
        //LoadModelFromTextFile("Common/PowerPlantFiles/Section2.txt", powerPlantMaterial);
        //LoadModelFromTextFile("Common/PowerPlantFiles/Section3.txt", powerPlantMaterial);
        //LoadModelFromTextFile("Common/PowerPlantFiles/Section4.txt", powerPlantMaterial);
        //LoadModelFromTextFile("Common/PowerPlantFiles/Section5.txt", powerPlantMaterial);
        //LoadModelFromTextFile("Common/PowerPlantFiles/Section6.txt", powerPlantMaterial);
        //LoadModelFromTextFile("Common/PowerPlantFiles/Section7.txt", powerPlantMaterial);
        //LoadModelFromTextFile("Common/PowerPlantFiles/Section8.txt", powerPlantMaterial);
        //LoadModelFromTextFile("Common/PowerPlantFiles/Section9.txt", powerPlantMaterial);
        LoadModelFromTextFile("Common/PowerPlantFiles/Section10.txt", powerPlantMaterial);
        //LoadModelFromTextFile("Common/PowerPlantFiles/Section11.txt", powerPlantMaterial);
        //LoadModelFromTextFile("Common/PowerPlantFiles/Section12.txt", powerPlantMaterial);
        //LoadModelFromTextFile("Common/PowerPlantFiles/Section13.txt", powerPlantMaterial);
        //LoadModelFromTextFile("Common/PowerPlantFiles/Section14.txt", powerPlantMaterial);
        //LoadModelFromTextFile("Common/PowerPlantFiles/Section15.txt", powerPlantMaterial);
        //LoadModelFromTextFile("Common/PowerPlantFiles/Section16.txt", powerPlantMaterial);
        //LoadModelFromTextFile("Common/PowerPlantFiles/Section17.txt", powerPlantMaterial);
        //LoadModelFromTextFile("Common/PowerPlantFiles/Section18.txt", powerPlantMaterial);
        //LoadModelFromTextFile("Common/PowerPlantFiles/Section19.txt", powerPlantMaterial);
        //LoadModelFromTextFile("Common/PowerPlantFiles/Section20.txt", powerPlantMaterial);
        //LoadModelFromTextFile("Common/PowerPlantFiles/Section21.txt", powerPlantMaterial);
    } else // Load the model if we have it in command line //"Common/models/sphere.obj"
    {
        ModelSystem::getInstance().LoadModel(Global::loadFile, [&, powerPlantMaterial](std::vector<Model> models) {
            assert(models.size() == 1);
            const Model &model = models[0];
            sphere = model;
            sphere.material = powerPlantMaterial;
            scene.models.emplace_back(sphere);

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
    scene.directionalLights.push_back(sunLight);

    scene.mainCamera = std::make_unique<FpsCamera>();

    scene.mainCamera->LookAt({0, 0, -20}, {0, 0, 0});

}

void AssignmentThree::Resize(int width, int height) {
    if (width == 0 || height == 0)
        return;

    scene.mainCamera->Resize(width, height);
    gBuffer.RecreateGpuResources(width, height);
    lightBuffer.RecreateGpuResources(width, height, gBuffer);
}

void AssignmentThree::Draw(const Input &input, float deltaTime, float runningTime) {
    glm::vec3 zero(0);
    glm::vec3 one(1);
    dd::cross(glm::value_ptr(zero), 5);
    if (ImGui::CollapsingHeader("Assignment Three Options - ")) {
        static int TrigEachNode = 1000;
        ImGui::InputInt("Trig Each Node", &TrigEachNode);
        ImGui::NewLine();

        if (ImGui::SmallButton("Generate Oct Tree")) {
            octTree.reset(new OctTree(TrigEachNode, {factoryCollisionMesh}));
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Load Oct Tree from JSON")) {
            octTree.reset(OctTreeFromJSON());
            if (octTree) {
                Log("Successfully loaded oct tree from json file.");
            }
        }
        ImGui::InputInt("OctTree depth", &OctTreeNode::MAX_DEPTH);

        if (octTree) {
            octTree->ImGuiSettings();
            if (ImGui::SmallButton("Serialize Oct Tree")) {
                SerializeOctTree(octTree.get());
            }
        }
        ImGui::NewLine();
        ImGui::NewLine();
        if (ImGui::SmallButton("Generate BSP Tree")) {
            bspTree.reset(new BSPTree(TrigEachNode, {factoryCollisionMesh}));
        }

        ImGui::SameLine();
        if (ImGui::SmallButton("Load BSP Tree from JSON")) {
            bspTree.reset(BSTTreeFromJSON());
            if (bspTree) {
                Log("Successfully loaded oct tree from json file.");
            }
        }
        ImGui::SliderFloat("K value", &BSPNode::k_value, 0.0f, 1.0f);
        ImGui::InputInt("BSP depth", &BSPNode::MAX_DEPTH);
        ImGui::InputInt("Random Split Lower Bound", &BSPNode::Good_Enough_Score);
        ImGui::InputInt("Plane Split Sampling Size", &BSPNode::Plane_Sample_Size);

        if (bspTree) {
            bspTree->ImGuiSettings();
            if (ImGui::SmallButton("Serialize BSP Tree")) {
                SerializeBSTTree(bspTree.get());
            }
        }
    }
    ImGui::NewLine();

    Update(input, deltaTime);
    for (auto &dirLight: scene.directionalLights) {
        dirLight.worldDirection = glm::rotateY(dirLight.worldDirection, deltaTime);
    }

    scene.mainCamera->CommitToGpu();
    geometryPass.Draw(gBuffer, scene);
    gBuffer.RenderGui("GBuffer");
    lightPass.Draw(lightBuffer, gBuffer, scene);
    LightPass::RenderGui(scene.directionalLights[0], lightBuffer);
    finalPass.Draw(gBuffer, lightBuffer, scene);

    if (octTree)
        octTree->ForwardRenderTree();
    if (bspTree)
        bspTree->ForwardRenderTree();
    // Now we do forward rendering

    // Copy depth buffer

    // Forward rendering
    {
        forwardRendering.Draw(scene);
    }
}

AssignmentThree::AssignmentThree() : transformSystem(TransformSystem::getInstance()) {

}

void AssignmentThree::LoadModelFromTextFile(std::string fileName, Material *mat) {
    std::ifstream ifstream(fileName);
    if (!ifstream.is_open() || ifstream.fail()) {
        LogError("Reading of file %s failed. Program ending", fileName.c_str());
        return;
    }

    // To emulate the 'directory'
    std::filesystem::path directory(fileName);
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

    while (std::getline(ifstream, line)) {
        // Read each line, each line describes the model.
        // Load each line as a model, add the material to them, and then push back.
        Log("Starting to load model '%s'", (direc + line).c_str());
        ModelSystem::getInstance().LoadModel(direc + line, [&, mat, numberOfLines](std::vector<Model> models) {
            assert(models.size() == 1);
            Model &model = models[0];
            model.material = mat;
            Transform &transform = TransformSystem::getInstance().Get(model.transformID);
            transform.SetLocalScale(this->powerPlantScale);
            TransformSystem::getInstance().UpdateMatrices(model.transformID);
            scene.models.emplace_back(model);

            Log("Loading of %s is a success!", model.name.c_str());
            scene.debugSystem.AddDebugModel(DebugSystem::Face_Normal, model.faceNormal);
            scene.debugSystem.AddDebugModel(DebugSystem::Vertex_Normal, model.vertexNormal);

            auto &LoadedModels = ModelSystem::getInstance().GetAllLoadedModels();
            auto iterator = LoadedModels.find(model.name);
            if (iterator != LoadedModels.end()) {
                const LoadedModel &loadedModel = iterator->second[0];
                factoryCollisionMesh.AddModel(loadedModel, model.transformID);
            }
        });
    }
}

void AssignmentThree::AddShape(std::shared_ptr<Shape> shape) {
    Shapes.emplace_back(shape);
}

void AssignmentThree::Update(const Input &input, float dt) {
    scene.mainCamera->Update(input, dt);
    DebugDrawSystem::getInstance().Update(scene);

    // Broadphase Collision
    static std::vector<
            std::tuple<
                    std::shared_ptr<Shape>, std::shared_ptr<Shape>, Shapes::Collision>
    > Collided;
    Collided.reserve(1000);

    for (int i = 0; i < Shapes.size(); ++i) {
        for (int j = i + 1; j < Shapes.size(); ++j) {
            Shapes::Collision collision;
            if (Shapes::CheckCollision(*Shapes[i], *Shapes[j], collision)) {
                Collided.emplace_back(std::make_tuple(Shapes[i], Shapes[j], collision));
            }
        }
    }

    // All collided are in Collided
    for (int i = 0; i < Collided.size(); ++i) {
        std::shared_ptr<Shape> &a = std::get<0>(Collided[i]);
        std::shared_ptr<Shape> &b = std::get<1>(Collided[i]);
        Shapes::Collision &collision = std::get<2>(Collided[i]);
        Log("%s and %s are colliding! \n", typeid(a).name(), typeid(b).name());
    }
    Collided.clear();
}

void AssignmentThree::SerializeOctTree(const OctTree *octTree) const {
    using namespace rapidjson;

    // Open File and write to it
    static const char *octTreeFileName = "octree.json";
    FILE *file = fopen(octTreeFileName, "w");
    if (!file) {
        Log("Error! %s cannot be opened for serializing!", octTreeFileName);
        return;
    }

    // Assume octtree is not null. Serialize to writer
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);
    octTree->Serialize(writer);

    // Writer outputs to file
    fprintf(file, "%s", sb.GetString());
    Log("Successfully serialized oct tree to %s", octTreeFileName);
    fclose(file);
}

OctTree *AssignmentThree::OctTreeFromJSON() {

    using namespace rapidjson;

    // Open File and write to it
    static const char *octTreeFileName = "octree.json";
    std::ifstream ifs(octTreeFileName, std::ifstream::in);

    if (!ifs.good()) {
        Log("Error! %s cannot be read for deserializing!", octTreeFileName);
        return nullptr;
    }


    const int MAX_LEN = 256 * 2;
    char buffer[MAX_LEN] = {0};
    // -1 to allow room for NULL terminator for really long string
    std::string json;
    json.reserve(1024 * 84409); // OMEGALUL
    if (std::getline(ifs, json)) {
        Document d;
        d.Parse(json.c_str());
        return new OctTree(d);
    } else {
        Log("Processing of file went wrong!");
        return nullptr;
    }
}

void AssignmentThree::SerializeBSTTree(const BSPTree *bstTree) const {
    using namespace rapidjson;

    // Open File and write to it
    static const char *octTreeFileName = "bsttree.json";
    FILE *file = fopen(octTreeFileName, "w");
    if (!file) {
        Log("Error! %s cannot be opened for serializing!", octTreeFileName);
        return;
    }

    // Assume octtree is not null. Serialize to writer
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);
    bstTree->Serialize(writer);

    // Writer outputs to file
    fprintf(file, "%s", sb.GetString());
    Log("Successfully serialized bst tree to %s", octTreeFileName);
    fclose(file);
}

BSPTree *AssignmentThree::BSTTreeFromJSON() {
    using namespace rapidjson;

    // Open File and write to it
    static const char *bstTreeFileName = "bsttree.json";
    std::ifstream ifs(bstTreeFileName, std::ifstream::in);

    if (!ifs.good()) {
        Log("Error! %s cannot be read for deserializing!", bstTreeFileName);
        return nullptr;
    }


    // -1 to allow room for NULL terminator for really long string
    std::string json;
    json.reserve(1024 * 84409); // OMEGALUL
    if (std::getline(ifs, json)) {
        Document d;
        ParseResult ok = d.Parse(json.c_str());
        if (!ok) {
            Log("Error parsing %s to JSON", bstTreeFileName);
            return nullptr;
        }

        return new BSPTree(d);
    } else {
        Log("Processing of file went wrong!");
        return nullptr;
    }
}

