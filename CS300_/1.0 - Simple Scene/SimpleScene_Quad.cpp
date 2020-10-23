/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: SimpleScene_Quad.cpp
 * Purpose: The quad scene with the obj file loaded and spheres and stuff.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: RolandShum_CS300_1
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 3rd October 2020
 * End Header --------------------------------------------------------*/

#include "stdafx.h"
#include "SimpleScene_Quad.h"
#include "Affine.h"
#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/Camera.h"
#include "ObjLoader.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer.h"
#include "Object.h"
#include "ObjectManager.h"

static const glm::vec4 O  = Math::point(0,0,0),
                       EX = Math::vector(1,0,0),
                       EY = Math::vector(0,1,0),
                       EZ = Math::vector(0,0,1);


static char ModelPath[] = "../../Common/models/";
extern std::string GLOBAL_OBJFILE; // Located in GLAppLication

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void SimpleScene_Quad::SetupNanoGUI(GLFWwindow *pWindow)
{
    pWindow = nullptr;
}


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
SimpleScene_Quad::~SimpleScene_Quad()
{
    initMembers();
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
SimpleScene_Quad::SimpleScene_Quad(int windowWidth, int windowHeight) : Scene(windowWidth, windowHeight),
//                                                                        programID(0), vertexbuffer(0), VertexArrayID(0),
                                                                        angleOfRotation(0.0f), mainCamera(CreateUniquePtr<Camera>(
                                                                                O + 10.0f * EZ, -EZ, EY, 45, windowWidth / (float)windowHeight, 0.1f, 1000
                                                                                ))
{
    initMembers();
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-unused-return-value"
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void SimpleScene_Quad::initMembers()
{
    geometryBuffer.clear();
    angleOfRotation = 0.0f;
    objManager = CreateUniquePtr<ObjectManager>();
}
#pragma clang diagnostic pop

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void SimpleScene_Quad::CleanUp()
{

}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void SimpleScene_Quad::SetupBuffers()
{
    // Load the obj file
    std::string obj;

    // Load cube2.obj if obj not provided
    if(GLOBAL_OBJFILE.empty())
        obj = "cube2.obj";
    else { // Else load the global one
        obj = GLOBAL_OBJFILE;
    }
    std::string FullPath = ModelPath + obj;
    auto mesh = ObjLoader::LoadObj(FullPath.c_str());
    // Check if mesh was loaded
    if(mesh)
    {
        mesh = std::move(mesh.value());
    }
    // Setup the buffers for the OBJ object
    objLoadedObject->SetVAO(VertexArray::Create(), diffuseShader);
    objLoadedObject->AddShaderPass(generateNormalShader);

    // Buffer names are hardcoded, but not that impt as long as order is correct.
    mesh.value()->
            SetVerticeBuffer(objLoadedObject->GetVAO()).
            GenerateVertexColor(objLoadedObject->GetVAO()).
            CalcVertexNormal(objLoadedObject->GetVAO()).
            SetVertexNormal(objLoadedObject->GetVAO()).
            SetIndexBuffer(objLoadedObject->GetVAO());

    // Now we create sphere objects. We create one template, and point
    // the other spheres at it.
    sphereObject[0]->SetVAO(VertexArray::Create(), diffuseShader);
    sphereObject[0]->AddShaderPass(generateNormalShader);

    auto sphereMesh = ObjLoader::CreateSphere(0.5f, 6);
    sphereMesh.value()->
        SetVerticeBuffer(sphereObject[0]->GetVAO()).
        GenerateRandomColors(sphereObject[0]->GetVAO()).
        SetVertexNormal(sphereObject[0]->GetVAO()).
        SetIndexBuffer(sphereObject[0]->GetVAO());

    for(int i = 1; i < 8; i++)
    {
        sphereObject[i]->SetVAO(sphereObject[0]->GetVAO(), diffuseShader);
        sphereObject[i]->AddShaderPass(generateNormalShader);
    }

    // Now we create a line object.
    sphereLineObject->SetVAO(VertexArray::Create(VertexArray::PrimitiveType::Lines), lineShader);

    // Generate the points
    auto sphereLineMesh = ObjLoader::CreateCircularLine(1.0f, 30);
    UniquePtr<Mesh> meshPtr = std::move(sphereLineMesh.value());

    meshPtr->SetVerticeBuffer(sphereLineObject->GetVAO());
    meshPtr->SetIndexBuffer(sphereLineObject->GetVAO());

    std::string quadPath = std::string(ModelPath) + "quad.obj";
    UniquePtr<Mesh> quadMesh = std::move(ObjLoader::LoadObj(quadPath.c_str()).value());
    planeObject->SetVAO(VertexArray::Create(), diffuseShader);
    quadMesh->SetVerticeBuffer(planeObject->GetVAO()).
            GenerateRandomColors(planeObject->GetVAO()).
            CalcVertexNormal(planeObject->GetVAO()).
            SetVertexNormal(planeObject->GetVAO()).
            SetIndexBuffer(planeObject->GetVAO());


    return;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SimpleScene_Quad::Init()
{
    // Create and compile our GLSL program from the shaders
    static const std::string ShaderPath = "../Shaders/";

    diffuseShader = Shader::Create("PhongShader",
                                 ShaderPath + "DiffuseShaderPhong.vert",
                                 ShaderPath + "DiffuseShaderPhong.frag");

    generateNormalShader = Shader::Create("generateNormalShader",
                                          ShaderPath + "NormalGenerator.vert",
                                          ShaderPath + "NormalGenerator.frag",
                                          ShaderPath + "NormalGenerator.geom");

    lineShader = Shader::Create("lineShader",
                                ShaderPath + "LineShader.vert",
                                ShaderPath + "LineShader.frag");

    phongLighting = Shader::Create("Phong Lighting",
                                ShaderPath + "Assignment2/PhongLighting.vert",
                                ShaderPath + "Assignment2/PhongLighting.frag");

    shaderLibrary.Add("Diffuse Shader", diffuseShader);
    shaderLibrary.Add("Generate Normal Geo Shader", generateNormalShader);
    shaderLibrary.Add("Line Shader", lineShader);
    shaderLibrary.Add("Phong Lighting", phongLighting);

    auto rotateScaleLambda = [&](Object& obj, glm::mat4 objTransform)-> void
    {
        // Draw the VAO !
        // T * R * S * Vertex
        glm::mat4 modelMat = glm::mat4(1.0f);
        glm::vec3 scaleVector = glm::vec3(1.0f);
        glm::vec3 centroid = glm::vec3( 0,0,0);

        modelMat = objTransform * glm::translate( centroid ) *
                   glm::rotate(angleOfRotation, glm::vec3(0.0f, 1.0f, 1.0f)) *
                   glm::scale( scaleVector );

        obj.SetTransform(modelMat);
    };

    // Create OBJ object.
    objLoadedObject = objManager->CreateObject();
    objLoadedObject->SetUpdate(rotateScaleLambda);

    // Create Sphere Objects
    for(int i = 0; i < 8; ++i)
    {
        auto sphereUpdate = [&, i](Object& obj, glm::mat4 parentMatrix)-> void
        {
            glm::mat4 modelMat = glm::mat4(1.0f);
            glm::vec3 scaleVector = glm::vec3(1.0f);
            glm::vec3 centroid = glm::vec3( 0,0,3);

            modelMat = parentMatrix * glm::scale( scaleVector ) *
                       glm::rotate(angleOfRotation + (2 * PI / 8) * i, glm::vec3(0.0f, 1.0f, 0.0f)) *
                       glm::translate( centroid );

            obj.SetTransform(modelMat);
        };
        sphereObject[i] = objManager->CreateObject();
        sphereObject[i]->SetUpdate(sphereUpdate);
    }

    // Create sphere line object.
    auto sphereLineObjectUpdate = [&](Object& obj, glm::mat4 objTransform)-> void
    {
        // Draw the VAO !
        // T * R * S * Vertex
        glm::mat4 modelMat = glm::mat4(1.0f);
        glm::vec3 scaleVector = this->spherePathScale;
        glm::vec3 centroid =  this->spherePathPosition;

        modelMat = objTransform * glm::translate( centroid ) *
                   glm::rotate(angleOfRotation, glm::vec3(0.0f, 1.0f, 0.0f)) *
                   glm::scale( scaleVector );

        obj.SetTransform(modelMat);
    };
    this->sphereLineObject = objManager->CreateObject();
    this->sphereLineObject->SetUpdate(sphereLineObjectUpdate);

    auto planeObjectUpdate = [&](Object& obj, glm::mat4 objTransform)-> void {
        // Draw the VAO !
        // T * R * S * Vertex
        glm::mat4 modelMat = glm::mat4(1.0f);
        glm::vec3 scaleVector = this->planeScale;
        glm::vec3 centroid = this->planePosition;

        modelMat = objTransform * glm::translate(centroid) *
                   glm::rotate(3.14f / 2, glm::vec3(-1.0f, 0.0f, 0.0f)) *
                   glm::scale(scaleVector);

        obj.SetTransform(modelMat);
    };
    this->planeObject = objManager->CreateObject();
    this->planeObject->SetUpdate(planeObjectUpdate);

    SetupBuffers();

    return Scene::Init();
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SimpleScene_Quad::Render()
{
    RenderCommand::SetClearColor(glm::vec4(backgroundColor, 1.0f));
    RenderCommand::Clear();

    objLoadedObject->Update();
    //objLoadedObject->Renderable = false;
    sphereLineObject->Update();
    planeObject->Update();
    for(int i = 0; i < 8; ++i)
    {
        sphereObject[i]->Renderable = false;
        sphereObject[i]->Update();
    }

    diffuseShader->Bind();
    diffuseShader->SetFloat3("lightColor", lightColor);
    diffuseShader->SetFloat3("lightDirection", lightDirection);

    phongLighting->Bind();
    phongLighting->SetFloat3("lightColor", lightColor);
    phongLighting->SetFloat3("lightDirection", lightDirection);

    objManager->RenderAllObject();

    // Abuse the state nature of opengl. We change the shader here, and it'll reflect in the
    // next frame
    generateNormalShader->Bind();
    generateNormalShader->SetFloat4("lineColor", lineColor);
    generateNormalShader->SetInt("showNormals", showNormals);

    return 0;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SimpleScene_Quad::postRender()
{
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    angleOfRotation += 0.01f;
    return 0;
}

int SimpleScene_Quad::preRender() {

    Renderer::BeginScene(*this->mainCamera.get());

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    if (ImGui::Begin("Assignment 1"))
    {
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
        if (ImGui::CollapsingHeader("Background color")) {
            ImGui::ColorEdit3("Light Color",
                                (float *) (&(this->backgroundColor)));
        }
        if (ImGui::CollapsingHeader("Light Direction")) {
            ImGui::DragFloat3("Vec3 Light Dir",
                              (float *) (&(this->lightDirection)));
        }
        if (ImGui::CollapsingHeader("Light Color")) {
            ImGui::ColorEdit3("Light Color",
                                (float *) (&(this->lightColor)));

        }
        if (ImGui::CollapsingHeader("Display Normals?")) {
            ImGui::RadioButton("Don't Show", &showNormals, 0);
            ImGui::RadioButton("Show Face Normals", &showNormals, 1);
            ImGui::RadioButton("Show Vertex Normals", &showNormals, 2);
        }
        if(ImGui::CollapsingHeader("Plane pos"))
        {
            ImGui::DragFloat3("Vec3 Pos ",
                              (float *) (&(this->planePosition)));
            ImGui::DragFloat3("Vec3 Scale",
                              (float *) (&(this->planeScale)));

        }
        if(ImGui::CollapsingHeader("Shader Library"))
        {
            ImGui_ShaderLibrary(shaderLibrary);
        }


    }
    ImGui::End();
    ImGui::Render();
    return 0;
}

void ImGui_ShaderLibrary(ShaderLibrary & shaderlib)
{
    if (ImGui::TreeNode("Shaders")) {
        for (auto &pair : shaderlib.Shaders) {
            auto &shader = pair.second;
            auto &name = pair.first;
            if (ImGui::TreeNode(name.c_str())) {
                auto &tuple = shaderlib.ShaderFilePaths[name];
                std::string const &vertexPath = std::get<0>(tuple);
                std::string const &fragPath = std::get<1>(tuple);
                std::string const &geoPath = std::get<2>(tuple);

                ImGui::SameLine();
                if (ImGui::Button("Reload")) {
                    shader->Reload(vertexPath, fragPath, geoPath);
                }

                ImGui::Text("Vertex Path: %s", vertexPath.c_str());
                ImGui::Text("Frag Path: %s", fragPath.c_str());
                ImGui::Text("Geo Path: %s", geoPath.c_str());
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }
}