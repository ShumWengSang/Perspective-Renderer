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
#include "Model.h"
#include "TransformSystem.h"
#include "Animation.h"
#include "Animator.h"
#include "Phong_AnimatedMaterial.h"
#include <ShaderLocations.h>
#include <implot_internal.h>
#include "CameraUniforms.h"
#include "BezierCurve.h"

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

    PowerPlantMaterial *powerPlantMaterial = new PowerPlantMaterial();
    powerPlantMaterial->ReadMaterialFromFile("Common/PowerPlantFiles/");
    MaterialSystem::getInstance().ManageMaterial(powerPlantMaterial);

    // alien
    const std::string modelAndAnimationName = "Common/Animation/alien.fbx";
    model = new Model(modelAndAnimationName.c_str());
    model->material = phongAnimated;
    model->transformID = TransformSystem::getInstance().Create();
    Transform &trans = TransformSystem::getInstance().Get(model->transformID);
    // trans.SetLocalPosition(0, 0, -4500);
    // trans.SetLocalDirection(0, 180, 0);
    trans.SetLocalScale(0.1);
    TransformSystem::getInstance().UpdateMatrices(model->transformID);

    //Model *cube = new Model("Common/cube.obj");
    //cube->material = powerPlantMaterial;
    //cube->transformID = TransformSystem::getInstance().Create();
    //Transform& cubeTrans = TransformSystem::getInstance().Get(cube->transformID);
    //cubeTrans.SetLocalPosition(0, -50, 0);
    //cubeTrans.SetLocalDirection(0, 0, 0);
    //cubeTrans.SetLocalScale(10000000);
    //TransformSystem::getInstance().UpdateMatrices(cube->transformID);
    //scene.entities.emplace_back(Entity{ cube, nullptr });

    // Animation
    // animation = new Animation("Common/alien.fbx", model);

    Assimp::Importer importer;
    const aiScene *ASSIMPScene = importer.ReadFile(modelAndAnimationName, aiProcess_Triangulate
                                                                          | aiProcess_FlipUVs |
                                                                          aiProcess_LimitBoneWeights);
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

    static int selected = -1;
    if (ImGui::CollapsingHeader("Animation System")) {
        if (ImGui::TreeNode("Select Animation to Play")) {

            for (int n = 0; n < animation.size(); n++) {
                char buf[64];
                sprintf(buf, "#%i: ", n + 1);
                bool selectable = ImGui::Selectable(buf, selected == n);
                ImGui::SameLine();
                ImGui::TextColored({0.1, 0.7, 0.1, 1.0}, "[%s]", animation[n]->GetName().c_str());

                if (selectable) {
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

    // as a test, generate random data
    static bool once = true;
    if (once) {
        once = false;

        bezierKnotPoints.emplace_back(glm::vec3(-140,  -10, 542));
        bezierKnotPoints.emplace_back(glm::vec3(394, -10, 1596));
        bezierKnotPoints.emplace_back(glm::vec3(843, -10, 1428));
        bezierKnotPoints.emplace_back(glm::vec3(1240, -10, 748));
        bezierKnotPoints.emplace_back(glm::vec3(-214, -10, -335));
        bezierKnotPoints.emplace_back(glm::vec3(120, -10, -935));
        bezierKnotPoints.emplace_back(glm::vec3(772, -10, -707));
        bezierKnotPoints.emplace_back(glm::vec3(491, -10, -157));
        bezierKnotPoints.emplace_back(glm::vec3(61, -10, 828));

        bezierCurve.AddPoints(bezierKnotPoints);
		generatedPlotPoints.reserve(1000);
	}

	char label[32];
	{
		static float circleRun = 0.0f;
		circleRun += deltaTime * 0.1f;
		circleRun = std::fmod(circleRun, 1.0f);
		glm::vec3 color{ 1, 0,0 };
		dd::sphere(glm::value_ptr(bezierCurve.Interpolate(circleRun)), glm::value_ptr(color), 50);
        auto& trans = TransformSystem::getInstance().Get(model->transformID);
        trans.SetLocalPosition(bezierCurve.Interpolate(circleRun));
        TransformSystem::getInstance().UpdateMatrices(model->transformID);
	}


    if (ImGui::CollapsingHeader("Path")) {
        ImPlotAxisFlags ax_flags = ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks;

        generatedPlotPoints.clear();


        const std::vector<glm::vec3>& controlPoints = bezierCurve.GetGeneratedControlPoints();
        // Render Knots
        auto& knots = bezierCurve.GetPoints();

        for (int i = 0; i < controlPoints.size(); i += 4)
        {
            if (controlPoints.size() % 4 != 0)
                DebugBreak();
            // Generate line from control points
            for (int j = 0; j < 100; ++j) {
                double t = j / 99.0;

                generatedPlotPoints.emplace_back(BezierCurve::Interpolate(t, controlPoints[i], controlPoints[i + 1], controlPoints[i + 2], controlPoints[i + 3]));
            }

        }
        static bool addPoints = false;
        ImGui::Checkbox("Add Points by click?", &addPoints);
        static bool movePoints = false;
        ImGui::Checkbox("Move Points by dragging", &movePoints);

        if (ImGui::TreeNode("Show knots"))
        {
            for (int i = 0; i < knots.size(); ++i)
            {
                ImGui::Text("Knot %i: [%f, %f, %f]", i, knots[i].x, knots[i].y, knots[i].z);
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Generated Control Points"))
        {
            for (int i = 0; i < controlPoints.size(); ++i)
            {
                ImGui::Text("Control Point %i: [%f, %f, %f]", i, controlPoints[i].x, controlPoints[i].y, controlPoints[i].z);
                if ((i + 1) % 4 == 0)
                {
                    ImGui::NewLine();
                }
            }

            ImGui::TreePop();
        }

        if (ImPlot::BeginPlot("Bezier", ImVec2(-1, 0), ImPlotFlags_NoTitle | ImPlotFlags_NoLegend | ImPlotFlags_NoMenus | ImPlotFlags_NoBoxSelect)) {
            ImPlot::SetupAxes(0, 0, ax_flags, ax_flags);
            ImPlot::SetupAxesLimits(-1000, 1000, -1000, 1000);
 

            // Generate line in the world 
            for (int i = 1; i < generatedPlotPoints.size(); i++)
            {
                glm::vec3 color{ 0, 1, 0 };
                dd::line(glm::value_ptr(generatedPlotPoints[i - 1]), glm::value_ptr(generatedPlotPoints[i]), glm::value_ptr(color));
            }


            for(int i = 0; i < knots.size(); ++i)
            {
                glm::dvec3& line = knots[i]; 
                if(!movePoints)
                    ImPlot::DragPoint(i, &line.x, &line.z, ImVec4(0, 0.9f, 0, 1), 4, ImPlotDragToolFlags_NoInputs);
                else
                    ImPlot::DragPoint(i, &line.x, &line.z, ImVec4(0, 0.9f, 0, 1), 4);
                glm::vec3 color {1,0,0};
                glm::vec3 knotP = knots[i];
                dd::sphere(glm::value_ptr(knotP), glm::value_ptr(color), 25);
            }

            // Set the points
            ImPlot::SetNextLineStyle(ImVec4(0, 0.9f, 0.8, 0.5), 2);
            // ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
            ImPlot::PlotLine("##bez", &generatedPlotPoints[0].x, &generatedPlotPoints[0].z, generatedPlotPoints.size(), 0,
                             sizeof(glm::vec3));



            ImPlotPlot &plot   = *ImPlot::GetCurrentContext()->CurrentPlot;
            ImPlotAxis& y_axis = plot.YAxis(0);
            ImPlotAxis& x_axis = plot.XAxis(0);
            double u = x_axis.PixelsToPlot(ImGui::GetIO().MousePos.x);
            double v = y_axis.PixelsToPlot(ImGui::GetIO().MousePos.y);


            if (plot.Hovered && input.WasButtonPressed(GLFW_MOUSE_BUTTON_1))
            {
                if(addPoints)
                { 
                    bezierCurve.AddPoint(glm::vec3(u, -10.0, v));
                }
            }
            else if (plot.Hovered && input.WasButtonPressed(GLFW_MOUSE_BUTTON_2))
            {
                for(int i = 0; i < bezierKnotPoints.size(); ++i)
                {
                    std::cout << glm::to_string(bezierKnotPoints[i]) << '\n';
                }
                std::cout << std::endl;
            }
            ImPlot::EndPlot();
        }
    }
}



