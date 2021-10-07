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
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "Model.h"
#include "TransformSystem.h"
#include "Animation.h"
#include "Animator.h"
#include "Phong_AnimatedMaterial.h"
#include <ShaderLocations.h>
#include "CameraUniforms.h"

namespace {
    Scene scene{};
    Model *model;
    std::vector<Animation *> animation;
    Animator *animator;

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
    settings.window.size = {1600, 900};
    settings.window.resizeable = false;
    settings.context.msaaSamples = 1;

    return settings;
}

void CS460AssignmentOne::Init() {
    // Load power plant material
    PhongAnimatedMaterial *phongAnimated = new PhongAnimatedMaterial();
    phongAnimated->ReadMaterialFromFile("Common/PowerPlantFiles/");
    MaterialSystem::getInstance().ManageMaterial(phongAnimated);


    const std::string modelAndAnimationName = "Common/fbx/Dragon_Baked_Actions_fbx_6.1_ASCII.fbx";
    model = new Model(modelAndAnimationName.c_str());
    model->material = phongAnimated;
    model->transformID = TransformSystem::getInstance().Create();
    Transform &trans = TransformSystem::getInstance().Get(model->transformID);
    trans.SetLocalDirection(-90, 180, 0);
    TransformSystem::getInstance().UpdateMatrices(model->transformID);


    // Animation
    // animation = new Animation("Common/alien.fbx", model);

    Assimp::Importer importer;
    const aiScene *ASSIMPScene = importer.ReadFile(modelAndAnimationName, aiProcess_Triangulate);
    // Get the number of animation
    if (ASSIMPScene) {
        int numOfAnimations = ASSIMPScene->mNumAnimations;
        for (int i = 0; i < numOfAnimations; ++i) {
            animation.emplace_back(new Animation(ASSIMPScene, ASSIMPScene->mAnimations[i], model));
        }
    } else {
        LogError("FBX has no animations!");
    }

    Animation *anim = animation.empty() ? nullptr : animation.back();
    animator = new Animator(anim);

    scene.entities.emplace_back(Entity{model, animator});


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

    scene.mainCamera->LookAt({0, 10, -50}, {0, 0, 0});

    VQSUniformBlock.BindBufferBase(BufferObjectType::Uniform, PredefinedUniformBlockBinding(VQSUniformBlock));
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
    animator->UpdateAnimation(deltaTime);


    const auto &transforms = animator->GetFinalBoneMatrices();
    // Prepare the data for UBO
    for (int i = 0; i < transforms.size(); ++i) {
        if (i >= MAX_BONES)
            throw std::runtime_error("Too many bones!");
        auto &vqs = transforms[i];
        ShaderStruct::VQS mem;
        mem.q = glm::vec4(vqs.q.v.x, vqs.q.v.y, vqs.q.v.z, vqs.q.s);
        mem.s = vqs.s;
        mem.v = vqs.v;
        VQSUniformBlock.memory[i] = mem;
    }
    // Send to UBO
    VQSUniformBlock.UpdateGpuBuffer();

    for (auto &dirLight: scene.directionalLights) {
        dirLight.worldDirection = glm::rotateY(dirLight.worldDirection, deltaTime);
    }
    scene.mainCamera->CommitToGpu();
    geometryPass.Draw(gBuffer, scene);
    lightPass.Draw(lightBuffer, gBuffer, scene);
    finalPass.Draw(gBuffer, lightBuffer, scene);
    forwardRendering.Draw(scene);

    // ImGui
    static int selected = -1;
    if (ImGui::CollapsingHeader("Animation System")) {
        if (ImGui::TreeNode("Select Animation to Play")) {

            for (int n = 0; n < animation.size(); n++) {
                char buf[64];
                sprintf(buf, "#%i: [%s]", n + 1, animation[n]->GetName().c_str());
                if (ImGui::Selectable(buf, selected == n)) {
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
            } else
                ImGui::TextColored({0.7, 0.1, 0.1, 1.0}, "Select an animation to view this");
            ImGui::TreePop();
        }
    }


}
