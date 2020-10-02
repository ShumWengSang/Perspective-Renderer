//
// Created by pushpak on 6/1/18.
//

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
    std::string obj = "rhino.obj";
    std::string FullPath = ModelPath + obj;
    auto mesh = ObjLoader::LoadObj(FullPath.c_str());
    // Check if mesh was loaded
    if(mesh)
    {
        mesh = std::move(mesh.value());
    }
    objLoadedObject->SetVAO(VertexArray::Create(), phongShader);
    faceNormalLine->SetVAO(VertexArray::Create(VertexArray::PrimitiveType::Lines), lineShader);
    vertexNormalLine->SetVAO(VertexArray::Create(VertexArray::PrimitiveType::Lines), lineShader);
    //faceNormalLine->VAO = VertexArray::Create(VertexArray::PrimitiveType::Lines);
    //vertexNormalLine->VAO = VertexArray::Create(VertexArray::PrimitiveType::Lines);
    // Buffer names are hardcoded, but not that impt as long as order is correct.
    mesh.value()->
            SetVerticeBuffer(objLoadedObject->GetVAO()).
            GenerateRandomColors(objLoadedObject->GetVAO()).
            CalcVertexNormal(objLoadedObject->GetVAO()).
            SetVertexNormal(objLoadedObject->GetVAO()).
            SetIndexBuffer(objLoadedObject->GetVAO());

    mesh.value()->GenerateFaceNormalLines(faceNormalLine->GetVAO())
    .GenerateVertexNormalLines(vertexNormalLine->GetVAO());

    sphereObject[0]->SetVAO(VertexArray::Create(), phongShader);
    sphereFaceNormalLine[0]->SetVAO(VertexArray::Create(VertexArray::PrimitiveType::Lines), lineShader);
    sphereVertexNormalLine[0]->SetVAO(VertexArray::Create(VertexArray::PrimitiveType::Lines), lineShader);

    auto sphereMesh = ObjLoader::CreateSphere(0.5f, 6);
    sphereMesh.value()->
        SetVerticeBuffer(sphereObject[0]->GetVAO()).
        GenerateRandomColors(sphereObject[0]->GetVAO()).
        SetVertexNormal(sphereObject[0]->GetVAO()).
        SetIndexBuffer(sphereObject[0]->GetVAO()).
        GenerateVertexNormalLines(sphereVertexNormalLine[0]->GetVAO());

    sphereMesh.value()->GenerateFaceNormalLines(sphereFaceNormalLine[0]->GetVAO());

    for(int i = 1; i < 8; i++)
    {
        sphereFaceNormalLine[i]->SetVAO(sphereFaceNormalLine[0]->GetVAO(), lineShader);
        sphereObject[i]->SetVAO(sphereObject[0]->GetVAO(), phongShader);
        sphereVertexNormalLine[i]->SetVAO(sphereVertexNormalLine[0]->GetVAO(), lineShader);
        //sphereFaceNormalLine[i]->VAO = sphereFaceNormalLine[0]->VAO;
        //sphereObject[i]->VAO = sphereObject[0]->VAO;
        //sphereVertexNormalLine[i]->VAO = sphereVertexNormalLine[0]->VAO;
    }
    return;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SimpleScene_Quad::Init()
{
    // Create and compile our GLSL program from the shaders
    phongShader = Shader::Create("PhongShader",
                                 "../DiffuseShaderPhong.vert",
                                 "../DiffuseShaderPhong.frag");

    lineShader = Shader::Create("lineShader",
                                "../LineShader.vert",
                                "../LineShader.frag");

    auto rotateScaleLambda = [&](Object& obj, glm::mat4 objTransform)->void
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

    objLoadedObject = objManager->CreateObject();
    faceNormalLine = objManager->CreateObject();
    vertexNormalLine = objManager->CreateObject();

    objLoadedObject->SetUpdate(rotateScaleLambda);
    faceNormalLine->SetUpdate(rotateScaleLambda);
    vertexNormalLine->SetUpdate(rotateScaleLambda);



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
        sphereFaceNormalLine[i] = objManager->CreateObject();
        sphereVertexNormalLine[i] = objManager->CreateObject();

        sphereObject[i]->SetUpdate(sphereUpdate);
        sphereFaceNormalLine[i]->SetUpdate(sphereUpdate);
        sphereVertexNormalLine[i]->SetUpdate(sphereUpdate);
    }



    SetupBuffers();

    return Scene::Init();
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SimpleScene_Quad::Render()
{
    RenderCommand::SetClearColor({0.5f,0.5f,0.5f,0.5f});
    RenderCommand::Clear();

    objLoadedObject->Update();
    faceNormalLine->Update();
    vertexNormalLine->Update();

    for(int i = 0; i < 8; ++i)
    {
        sphereObject[i]->Update();
        sphereFaceNormalLine[i]->Update();
        sphereVertexNormalLine[i]->Update();
    }

    phongShader->Bind();
    phongShader->SetFloat3("lightColor", lightColor);
    phongShader->SetFloat3("lightDirection", lightDirection);
            //modelMat = glm::identity<glm::mat4>();

    objManager->RenderAllObject();

    /////////////////////////// Render the obj and the spheres
    // Renderer::Submit(phongShader, objLoadedObject->VAO, objLoadedObject->GetTransformMatrix());
    //RenderObject(phongShader, *objLoadedObject);
    for(int i = 0; i < 8; ++i)
    {
        //RenderObject(phongShader, *sphereObject[i]);
//        Renderer::Submit(phongShader, sphereObject[i]->VAO, sphereObject[i]->GetTransformMatrix());
    }

    ///////////////////////// Render the normal lines!
    lineShader->Bind();
    lineShader->SetFloat4("lineColor", lineColor);
    //Renderer::Submit(lineShader, faceNormalLine->VAO, faceNormalLine->GetTransformMatrix());
//    RenderObject(lineShader, *faceNormalLine);
//    RenderObject(lineShader, *vertexNormalLine);

    for(int i = 0; i < 8; ++i)
    {
        //RenderObject(lineShader, *sphereFaceNormalLine[i]);
        //Renderer::Submit(lineShader, sphereFaceNormalLine[i]->VAO, sphereFaceNormalLine[i]->GetTransformMatrix());
    }
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

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
//    if (show_demo_window)
//        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {

        ImGui::Begin("Assignment 1");                          // Create a window called "Hello, world!" and append into it.

        ImGui::DragFloat3("Light Direction",
                          (float*)(&(this->lightDirection)));
        ImGui::ColorPicker3("Light Color",
                          (float*)(&(this->lightColor)));

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }
    ImGui::Render();
    return 0;
}
