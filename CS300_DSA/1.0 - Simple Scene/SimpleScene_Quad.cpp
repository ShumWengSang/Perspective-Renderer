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
#include "ObjLoader.h"
#include "Camera.h"
#include "Vertex.h"
#include "Shader.h"
#include "VertexArray.h"
#include "ProgramPipeline.h"
#include "Object.h"
#include "UniformBuffer.h"
#include "MyUBO.h"
#include "Texture.h"
#include "ImGuizmo.h"


static const glm::vec4 O  = Math::point(0,0,0),
                       EX = Math::vector(1,0,0),
                       EY = Math::vector(0,1,0),
                       EZ = Math::vector(0,0,1);

static const glm::vec4 LookAt = glm::normalize(-EZ + glm::vec4(0, -0.5f, 0, 0.0f));

static char ModelPath[] = "../../Common/models/";
static char TexturePath[] = "../../Common/textures/";
extern std::string GLOBAL_OBJFILE; // Located in GLAppLication
extern int texture_mode;
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
                                                                                O + 15.0f * EZ + 2.0f * EY, LookAt, EY, 45.0f, windowWidth / (float)windowHeight, 0.1f, 1000.0f
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
        obj = "sphere.obj";
    else { // Else load the global one
        obj = GLOBAL_OBJFILE;
    }
    std::string FullPath = ModelPath + obj;
    auto mesh = ObjLoader::LoadObj(FullPath);
    auto planeMesh = ObjLoader::LoadObj(ModelPath + std::string("quad.obj"));
    auto sphereMesh = ObjLoader::CreateSphere(0.3f, 8);
    auto sphereLineMesh = ObjLoader::CreateCircularLine(1.0f, 30);
    // Check if mesh was loaded
    if(!mesh)
    {
        std::cout << "Error! " << FullPath << " not loaded!" << std::endl;
    }
    if(!planeMesh)
    {
        std::cout << "Error! Quad.obj not loaded!" << std::endl;
    }

    planeMesh->get()->GenerateVertexNormals().GenerateColors({0.1f, 0.1f, 0.1f}).GenerateTexCoords(texture_mode, false);
    // We need to generate the normals and the colors
    mesh->get()->GenerateVertexNormals().GenerateColors({0.4f, 0.3f, 0.1f}).GenerateTexCoords(texture_mode, false);
    sphereMesh->get()->GenerateColors({0.3f, 0.7f, 0.9f}).GenerateTexCoords(texture_mode, false);

    // Create the format of the vertex
    /* vertex formatting information */
    std::vector<attribFormat> const vertexFormat =
            {
                    createAttribFormat<glm::vec3>(0, offsetof(Vertex, position)),
                    createAttribFormat<glm::vec3>(1, offsetof(Vertex, color)),
                    createAttribFormat<glm::vec3>(2, offsetof(Vertex, normal)),
                    createAttribFormat<glm::vec2>(3, offsetof(Vertex, texCoord))
            };

    // Create VAO, VBO, and IBO for mesh object
    {
        std::vector<Vertex> MeshVertices = Mesh::CreateVertexFromMesh(*mesh.value());
        auto const[vao_mesh, vbo_mesh, ibo_mesh] = createGeometry(MeshVertices, mesh->get()->Index, vertexFormat);
        meshArray = CreateSharedPtr<VertexArray>(vao_mesh);
        meshArray->PushBuffer({vbo_mesh, ibo_mesh});
        meshArray->SetIndexSize(mesh->get()->Index.size());
        VBOmesh = vbo_mesh;
    }
    // Create VAO for plane object
    {
        std::vector<Vertex> QuadVertices = Mesh::CreateVertexFromMesh(*planeMesh.value());
        auto const[vao_mesh, vbo_mesh, ibo_mesh] = createGeometry(QuadVertices, planeMesh->get()->Index, vertexFormat);
        planeArray = CreateSharedPtr<VertexArray>(vao_mesh);
        planeArray->PushBuffer({vbo_mesh, ibo_mesh});
        planeArray->SetIndexSize(planeMesh->get()->Index.size());
        VBOplaneMesh = vbo_mesh;
    }
    // Create sphere line
    {
        std::vector<Vertex> QuadVertices = Mesh::CreateVertexFromMeshDefaultLine(*sphereLineMesh.value());
        auto const[vao_mesh, vbo_mesh, ibo_mesh] = createGeometry(QuadVertices, sphereLineMesh->get()->Index, vertexFormat);
        sphereLine = CreateSharedPtr<VertexArray>(vao_mesh);
        sphereLine->PushBuffer({vbo_mesh, ibo_mesh});
        sphereLine->SetIndexSize(sphereLineMesh->get()->Index.size());
    }
    // Create sphere VAO
    {
        std::vector<Vertex> SphereVertices = Mesh::CreateVertexFromMesh(*sphereMesh.value());
        auto const[vao_mesh, vbo_mesh, ibo_mesh] = createGeometry(SphereVertices, sphereMesh->get()->Index, vertexFormat);
        sphereArray = CreateSharedPtr<VertexArray>(vao_mesh);
        sphereArray->PushBuffer({vbo_mesh, ibo_mesh});
        sphereArray->SetIndexSize(sphereMesh->get()->Index.size());
    }

    static const std::string ShaderPath = "../Shaders/";
    // Load the shaders
    {
        // Load default Shader
        auto const[default_pr, default_vert_shader, default_geo_shader, default_frag_shader] = Shader::CreateProgram(
                ShaderPath + "DefaultShader.vert", "", ShaderPath + "DefaultShader.frag");

        defaultPipeline = CreateSharedPtr<ProgramPipeline>(default_pr, "Default Shaders");

        defaultPipeline->PushShader(default_vert_shader, ProgramPipeline::ShaderType::Vertex);
        defaultPipeline->PushShader(default_frag_shader, ProgramPipeline::ShaderType::Fragment);
    }
    {
        // Load phong diffuse shader
        auto const[default_pr, default_vert_shader, default_geo_shader, default_frag_shader] = Shader::CreateProgram(
                ShaderPath + "DiffuseShaderPhong.vert", "", ShaderPath + "DiffuseShaderPhong.frag");

        PhongDiffuse = CreateSharedPtr<ProgramPipeline>(default_pr, "Phong Diffuse Shader");
        PhongDiffuse->PushShader(default_vert_shader, ProgramPipeline::ShaderType::Vertex);
        PhongDiffuse->PushShader(default_frag_shader, ProgramPipeline::ShaderType::Fragment);
    }
    // Load normal generator shaders
    {
        auto const[default_pr, default_vert_shader, default_geo_shader, default_frag_shader] = Shader::CreateProgram(
                ShaderPath + "NormalGenerator.vert", ShaderPath + "NormalGenerator.geom", ShaderPath + "NormalGenerator.frag");

        NormalLineGeoPipeline = CreateSharedPtr<ProgramPipeline>(default_pr, "Phong Diffuse Shader");
        NormalLineGeoPipeline->PushShader(default_vert_shader, ProgramPipeline::ShaderType::Vertex);
        NormalLineGeoPipeline->PushShader(default_geo_shader, ProgramPipeline::ShaderType::Geometry);
        NormalLineGeoPipeline->PushShader(default_frag_shader, ProgramPipeline::ShaderType::Fragment);
    }
    // Phong lighting shaders
    {
        auto const[default_pr, default_vert_shader, default_geo_shader, default_frag_shader] = Shader::CreateProgram(
                ShaderPath + "Assignment2/PhongLighting.vert", "", ShaderPath + "Assignment2/PhongLighting.frag");

        PhongLighting = CreateSharedPtr<ProgramPipeline>(default_pr, "Phong Diffuse Shader");
        PhongLighting->PushShader(default_vert_shader, ProgramPipeline::ShaderType::Vertex);
        PhongLighting->PushShader(default_frag_shader, ProgramPipeline::ShaderType::Fragment);
    }
    // Phong Shading shader
    {
        auto const[default_pr, default_vert_shader, default_geo_shader, default_frag_shader] = Shader::CreateProgram(
                ShaderPath + "Assignment2/PhongShading.vert", "", ShaderPath + "Assignment2/PhongShading.frag");

        PhongShading = CreateSharedPtr<ProgramPipeline>(default_pr, "Phong Diffuse Shader");
        PhongShading->PushShader(default_vert_shader, ProgramPipeline::ShaderType::Vertex);
        PhongShading->PushShader(default_frag_shader, ProgramPipeline::ShaderType::Fragment);
    }
    {
        // Blinn Phong
        auto const[default_pr, default_vert_shader, default_geo_shader, default_frag_shader] = Shader::CreateProgram(
                ShaderPath + "Assignment2/BlinnPhong.vert", "" "", ShaderPath + "Assignment2/BlinnPhong.frag");

        BlinnShading = CreateSharedPtr<ProgramPipeline>(default_pr, "Blinn-Phong Shader");
        // BlinnShading->PushShader(default_vert_shader, ProgramPipeline::ShaderType::Vertex);
        BlinnShading->PushShader(PhongShading->GetShader(ProgramPipeline::ShaderType::Vertex), ProgramPipeline::ShaderType::Vertex);
        BlinnShading->PushShader(default_frag_shader, ProgramPipeline::ShaderType::Fragment);
    }

    // Create UBO
    {
        MyUBO uboData{};
        auto const ubufferID = createBuffer(std::vector<MyUBO>{uboData});
        uniformBuffer = CreateSharedPtr<UniformBuffer<MyUBO>>(ubufferID, UBOBinding);
    }
    // Create light UBO
    {
        LightUBO lightData{};
        auto const ubufferID = createBuffer(std::vector<LightUBO>{lightData});
        lightUBO = CreateSharedPtr<UniformBuffer<LightUBO>>(ubufferID, LightUBOBinding);
    }

    // Load some textures
    {
        const std::string texturePath = TexturePath;
        auto const DiffuseID = Texture::CreateTexture2DFromFile(texturePath + "metal_roof_diff_512x512.png", Texture::STBI_rgb);
        auto const SpecularID = Texture::CreateTexture2DFromFile(texturePath + "metal_roof_spec_512x512.png", Texture::STBI_rgb);
        DiffuseTexture = CreateSharedPtr<Texture>(DiffuseID, DiffuseBinding);
        SpecularTexture = CreateSharedPtr<Texture>(SpecularID, SpecularBinding);

        DiffuseTexture->Bind();
        SpecularTexture->Bind();
    }

    this->quadMesh = std::move(planeMesh.value());
    this->mesh = std::move(mesh.value());
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SimpleScene_Quad::Init()
{
    SetupBuffers();

    meshObj = CreateSharedPtr<Object>();
    meshObj->SetModelMatrix(glm::translate( glm::vec3(0.0f) ) *
                            glm::rotate(cubeAngle, glm::vec3(0.0f, 1.0f, 0.0f)) *
                            glm::scale( glm::vec3(1.0f) ));
//    meshObj->SetUpdate([&](Object& obj)
//                       {
//                           glm::vec3 scaleVector = glm::vec3(1.0f);
//                           glm::vec3 centroid = meshPosition;
//                           obj.SetModelMatrix(glm::translate( centroid ) *
//                                      glm::rotate(cubeAngle, glm::vec3(0.0f, 1.0f, 0.0f)) *
//                                      glm::scale( scaleVector ));
//                       }); // angleOfRotation

    planeObj = CreateSharedPtr<Object>();
    planeObj->SetUpdate([&](Object& obj)
                       {
                           glm::vec3 scaleVector = planeScale;
                           glm::vec3 centroid = planePosition;
                           obj.SetModelMatrix( glm::translate( centroid ) *
                                        glm::rotate(-3.14f / 2, glm::vec3(1.0f, 0.0f, 0.0f)) *
                                        glm::scale( scaleVector ));
                       });

    for(int i = 0; i < 16; ++i) {
        sphereObj[i] = CreateSharedPtr<Object>();
        sphereObj[i]->SetUpdate([&, i](Object& obj)
                                {
                                        glm::mat4 modelMat = glm::mat4(1.0f);
                                        glm::vec3 scaleVector = glm::vec3(1.f);
                                        glm::vec3 centroid = glm::vec3(0, 0, 3);
                                        obj.SetModelMatrix(glm::scale(scaleVector) *
                                                           glm::rotate(angleOfRotation + (2 * PI / 16) * i + PI,
                                                                       glm::vec3(0.0f, 1.0f, 0.0f)) *
                                                           glm::translate(centroid));
                                });
    }

    sphereLineObj = CreateSharedPtr<Object>();
    sphereLineObj->SetModelMatrix(glm::translate( spherePathPosition ) *
                                  glm::rotate(angleOfRotation, glm::vec3(0.0f, 1.0f, 0.0f)) *
                                          glm::scale(spherePathScale));

    // Setup light colors and light UBO settings...
    LightUBO& lightUbo = lightUBO->GetUniformData();
    for(int i = 1 ; i < 16; i++)
    {
        lightUbo.lightInfo[i] = lightUbo.lightInfo[0];
        lightUbo.lightDiffuseColor[i] = lightUbo.lightDiffuseColor[0];
        lightUbo.lightAmbientColor[i] = lightUbo.lightAmbientColor[0];
        lightUbo.lightSpecularColor[i] = lightUbo.lightSpecularColor[0];
    }

    return Scene::Init();
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SimpleScene_Quad::Render()
{
    // Update light UBO
    LightUBO& lightUbo = lightUBO->GetUniformData();
    for(int i = 0; i < MAX_LIGHT; i++)
    {
        // Update light position and direction
        lightUbo.lightPos[i] = glm::column(sphereObj[i]->GetModelMatrix(), 3);
        lightUbo.lightDir[i] = glm::normalize(glm::column(meshObj->GetModelMatrixRef(),3) - lightUbo.lightPos[i]);
    }
    lightUbo.cameraPosition = mainCamera->eye();
    lightUBO->SendData();


    // Draw mesh for diffuse shader
    {
        if(useShader == 0) {
            PhongLighting->Bind();
            auto vertexProgram = PhongLighting->GetShader(ProgramPipeline::ShaderType::Vertex);
            vertexProgram->SetUniform(uniformAmbientColor, this->AmbientColor);
            vertexProgram->SetUniform(uniformEmissiveColor, this->EmissiveColor);
            DrawObject(meshArray, PhongLighting, meshObj);
            DrawObject(planeArray, PhongLighting, planeObj);
        }
        else if(useShader == 1) {
            PhongShading->Bind();
            auto fragmentProgram = PhongShading->GetShader(ProgramPipeline::ShaderType::Fragment);
            fragmentProgram->SetUniform(uniformAmbientColor, this->AmbientColor);
            fragmentProgram->SetUniform(uniformEmissiveColor, this->EmissiveColor);
            DrawObject(meshArray, PhongShading, meshObj);
            DrawObject(planeArray, PhongShading, planeObj);
        }
        else if(useShader == 2)
        {
            BlinnShading->Bind();
            auto fragmentProgram = BlinnShading->GetShader(ProgramPipeline::ShaderType::Fragment);
            fragmentProgram->SetUniform(uniformAmbientColor, this->AmbientColor);
            fragmentProgram->SetUniform(uniformEmissiveColor, this->EmissiveColor);
            DrawObject(meshArray, BlinnShading, meshObj);
            DrawObject(planeArray, BlinnShading, planeObj);
        }
    }


    {
        PhongDiffuse->Bind();
        auto vertexProgram = PhongDiffuse->GetShader(ProgramPipeline::ShaderType::Vertex);
        auto fragProgram = PhongDiffuse->GetShader(ProgramPipeline::ShaderType::Fragment);
        // Render the diffuse only pass.


        // Draw spheres
        for (int i = 0; i < (int)lightUBO->GetUniformData().numOfLight; i++) {
            fragProgram->SetUniform(uniformlightColor, glm::vec3(lightUBO->GetUniformData().lightDiffuseColor[i]));
            fragProgram->SetUniform(uniformLightDirection, glm::vec3(lightUBO->GetUniformData().lightDir[i]));
            DrawObject(sphereArray, PhongDiffuse, sphereObj[i]);
        }
    }

    // Swap pipeline to default pipeline
    {
        defaultPipeline->Bind();

        // Draw plane
//        DrawObject(planeArray, defaultPipeline, planeObj);
    }

    // Swap the pipeline to draw the normals for the cube
    {
        NormalLineGeoPipeline->Bind();
        DrawObject(meshArray, NormalLineGeoPipeline, meshObj);
        DrawObject(planeArray, NormalLineGeoPipeline, planeObj);
    }

    // Draw circular line
    {
        defaultPipeline->Bind();
        sphereLine->Bind();
        defaultPipeline->GetShader(ProgramPipeline::ShaderType::Vertex)->SetUniform(uniformModel, sphereLineObj->GetModelMatrix());
        glDrawElements(GL_LINES, sphereLine->GetIndexSize(), GL_UNSIGNED_INT, nullptr);
    }

    return 0;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SimpleScene_Quad::postRender()
{
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    if(SphereUpdate) {
        angleOfRotation += 0.01f;
    }
    return 0;
}

int SimpleScene_Quad::preRender() {

    // Update the data in the objects before anything
    meshObj->Update();
    planeObj->Update();
    for(int i = 0; i < 16; ++i)
    {
        sphereObj[i]->Update();
    }

    // Update view Projection UBO
    uniformBuffer->GetUniformData().Projection =  mainCamera->GetPerspectiveGLM();
    uniformBuffer->GetUniformData().View =  mainCamera->GetViewMatrixGLM();
    uniformBuffer->GetUniformData().NearFar.x = mainCamera->near();
    uniformBuffer->GetUniformData().NearFar.y = mainCamera->far();
    uniformBuffer->SendData();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    if (ImGui::Begin("Assignment 2"))
    {
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
        ImGui::Checkbox("Rotate light", &this->SphereUpdate);
        static int numOfLight = 1;
        numOfLight = (int)lightUBO->GetUniformData().numOfLight;
        ImGui::SliderInt("Number of lights", &numOfLight, 1, 16);
        lightUBO->GetUniformData().numOfLight = (float)numOfLight;
        ImGui::Columns(3);
        if(ImGui::Button("One Point light"))
        {
            useShader = 2;
            LightUBO& data = lightUBO->GetUniformData();
            data.numOfLight = 1.0f;
            data.lightInfo[0].x = 0.0f;
            data.lightDiffuseColor[0] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

        }
        ImGui::NextColumn();
        if(ImGui::Button("One Directional light"))
        {
            useShader = 2;
            LightUBO& data = lightUBO->GetUniformData();
            data.numOfLight = 1.0f;
            data.lightInfo[0].x = 1.0f;
            data.lightDiffuseColor[0] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

        }
        ImGui::NextColumn();
        if(ImGui::Button("One Spotlight light"))
        {
            useShader = 2;
            LightUBO& data = lightUBO->GetUniformData();
            data.numOfLight = 1.0f;
            data.lightInfo[0].x = 2.0f;
            data.lightDiffuseColor[0] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }
        ImGui::NextColumn();

        if(ImGui::Button("Scenario 1 - Point light"))
        {
            useShader = 2;
            // All lights of same type and parameter
            LightUBO& data = lightUBO->GetUniformData();
            data.numOfLight = 16.0f;
            for(int i = 0; i < 16; ++i)
            {
                data.lightInfo[i].x = 0.0f;
                data.lightDiffuseColor[i] = glm::vec4(1.0f, 0.5f, 0.5f, 1.0f);
            }
        }
        ImGui::NextColumn();
        if(ImGui::Button("Scenario 1 - Spotlight"))
        {
            useShader = 2;
            LightUBO& data = lightUBO->GetUniformData();
            data.numOfLight = 16.0f;
            for(int i = 0; i < 16; ++i)
            {
                data.lightInfo[i].x = 2.0f;
                data.lightDiffuseColor[i] = glm::vec4(1.0f, 0.5f, 0.5f, 1.0f);
            }
        }
        ImGui::NextColumn();
        if(ImGui::Button("Scenario 1 - Directional "))
        {
            useShader = 2;
            LightUBO& data = lightUBO->GetUniformData();
            data.numOfLight = 16.0f;
            for(int i = 0; i < 16; ++i)
            {
                data.lightInfo[i].x = 1.0f;
                data.lightDiffuseColor[i] = glm::vec4(1.0f, 0.5f, 0.5f, 1.0f);
            }
        }
        ImGui::NextColumn();
        // New column
        if(ImGui::Button("Scenario 2 - Point "))
        {
            useShader = 2;
            LightUBO& data = lightUBO->GetUniformData();
            data.numOfLight = 16.0f;
            for(int i = 0; i < 16; ++i)
            {
                data.lightInfo[i].x = 0.0f;
                data.lightDiffuseColor[i] = glm::vec4((rand() % 255) / 255.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f);
            }
        }
        ImGui::NextColumn();
        if(ImGui::Button("Scenario 2 - Directional "))
        {
            useShader = 2;
            LightUBO& data = lightUBO->GetUniformData();
            data.numOfLight = 16.0f;
            for(int i = 0; i < 16; ++i)
            {
                data.lightInfo[i].x = 1.0f;
                data.lightDiffuseColor[i] = glm::vec4((rand() % 255) / 255.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f);
            }
        }
        ImGui::NextColumn();
        if(ImGui::Button("Scenario 2 - Spotlight "))
        {
            useShader = 2;
            LightUBO& data = lightUBO->GetUniformData();
            data.numOfLight = 16.0f;
            for(int i = 0; i < 16; ++i)
            {
                data.lightInfo[i].x = 2.0f;
                data.lightDiffuseColor[i] = glm::vec4((rand() % 255) / 255.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f, (rand() % 255) / 255.0f);
            }
        }
        ImGui::Columns(1);
        if(ImGui::Button("Scenario 3"))
        {
            int i = 0;
            useShader = 2;
            LightUBO& data = lightUBO->GetUniformData();
            data.numOfLight = 16.0f;
            for(i; i < 6; ++i)
            {
                data.lightInfo[i].x = 0.0f;
                data.lightDiffuseColor[i] = glm::vec4((i * (255 / 6.0f)) / 255.0f, 0, 0, 1);
            }
            data.lightInfo[0].x = 2.0f;
            for(i; i < 12; ++i)
            {
                data.lightInfo[i].x = 0.0f;
                data.lightDiffuseColor[i] = glm::vec4(0, ((i - 6) * (255 / 6.0f)) / 255.0f, 0, 1);
            }
            data.lightInfo[7].x = 2.0f;
            for(i; i < 16; ++i)
            {
                data.lightInfo[i].x = 0.0f;
                data.lightDiffuseColor[i] = glm::vec4(0, 0, ((i - 12) * (255 / 4.0f)) / 255.0f, 1);
            }
            data.lightInfo[13].x = 2.0f;
        }
        if (ImGui::CollapsingHeader("Background color")) {
            ImGui::ColorEdit3("Background Color",
                              (float *) (&(this->backgroundColor)));
        }
        if(ImGui::CollapsingHeader("Object Transformation"))
        {
            EditTransform(*mainCamera.get(), meshObj->GetModelMatrixRef());
        }
        if(ImGui::CollapsingHeader("Plane Transformation"))
        {
            EditTransform(*mainCamera.get(), sphereLineObj->GetModelMatrixRef());
        }
        if(ImGui::CollapsingHeader("Plane pos"))
        {
            ImGui::DragFloat3("Vec3 Pos ",
                              (float *) (&(this->planePosition)));
            ImGui::DragFloat3("Vec3 Scale",
                              (float *) (&(this->planeScale)));

        }
        if (ImGui::CollapsingHeader("Display Normals?")) {
            ImGui::RadioButton("Don't Show", &showNormals, 0);
            ImGui::RadioButton("Show Face Normals", &showNormals, 1);
            ImGui::RadioButton("Show Vertex Normals", &showNormals, 2);

            NormalLineGeoPipeline->GetShader(ProgramPipeline::ShaderType::Geometry)->SetUniform(uniformFaceMode, showNormals);
            NormalLineGeoPipeline->GetShader(ProgramPipeline::ShaderType::Fragment)->SetUniform(uniformLineColor, this->lineColor);
        }
        if(ImGui::CollapsingHeader("UV Generation Mode?"))
        {
            static bool CPU_Generate = false;
            static bool GPU_Generate = false;
            static bool UseNormals = false;
            ImGui::Text("Press buttons to generate for CPU.");
            ImGui::Checkbox("CPU Generate?", &CPU_Generate);
            ImGui::Checkbox("GPU Generate?", &GPU_Generate);
            ImGui::Checkbox("Use Normals", &UseNormals);
            if(!GPU_Generate)
            {
                uniformBuffer->GetUniformData().modes.x = 0;
            }

            if (ImGui::CollapsingHeader("CPU UV Generation", &CPU_Generate))
            {
                ImGui::Text("Planar Mapping"); ImGui::SameLine();
                if(ImGui::Button("Generate Plane"))
                {
                    int mode = 0;
                    mesh->GenerateTexCoords(mode, UseNormals);
                    quadMesh->GenerateTexCoords(mode, UseNormals);
                    auto vertices = Mesh::CreateVertexFromMesh(*mesh);
                    glNamedBufferSubData(VBOmesh, 0, vertices.size() * sizeof(vertices[0]), vertices.data());
                    auto verticesQuad = Mesh::CreateVertexFromMesh(*quadMesh);
                    glNamedBufferSubData(VBOplaneMesh, 0, verticesQuad.size() * sizeof(verticesQuad[0]), verticesQuad.data());
                }
                ImGui::Text("Cylinder Mapping"); ImGui::SameLine();
                if(ImGui::Button("Generate Cylinder"))
                {
                    int mode = 1;
                    mesh->GenerateTexCoords(mode, UseNormals);
                    quadMesh->GenerateTexCoords(mode, UseNormals);
                    auto vertices = Mesh::CreateVertexFromMesh(*mesh);
                    glNamedBufferSubData(VBOmesh, 0, vertices.size() * sizeof(vertices[0]), vertices.data());
                    auto verticesQuad = Mesh::CreateVertexFromMesh(*quadMesh);
                    glNamedBufferSubData(VBOplaneMesh, 0, verticesQuad.size() * sizeof(verticesQuad[0]), verticesQuad.data());
                }
                ImGui::Text("Spherical Mapping"); ImGui::SameLine();
                if(ImGui::Button("Generate Sphere"))
                {
                    int mode = 2;
                    mesh->GenerateTexCoords(mode, UseNormals);
                    quadMesh->GenerateTexCoords(mode, UseNormals);
                    auto vertices = Mesh::CreateVertexFromMesh(*mesh);
                    glNamedBufferSubData(VBOmesh, 0, vertices.size() * sizeof(vertices[0]), vertices.data());
                    auto verticesQuad = Mesh::CreateVertexFromMesh(*quadMesh);
                    glNamedBufferSubData(VBOplaneMesh, 0, verticesQuad.size() * sizeof(verticesQuad[0]), verticesQuad.data());
                }
            }
            if (ImGui::CollapsingHeader("GPU UV Generate", &GPU_Generate))
            {
                int& GPUmode = uniformBuffer->GetUniformData().modes.x;
                uniformBuffer->GetUniformData().modes.y = (int)UseNormals;
                ImGui::RadioButton("Planer Mapping", &GPUmode, 1);
                ImGui::RadioButton("Cylindrical Mapping",&GPUmode, 2);
                ImGui::RadioButton("Spherical Mapping", &GPUmode, 3);
            }

        }
        if (ImGui::CollapsingHeader("Shader to use?")) {
            ImGui::RadioButton("Phong Lighting (Vertex)", &useShader, 0);
            ImGui::RadioButton("Phong Shading (Fragment)", &useShader, 1);
            ImGui::RadioButton("Blinn-Phong", &useShader, 2);
        }
        if(ImGui::CollapsingHeader("Mesh Materials"))
        {
            ImGui::ColorEdit3("Ambient Color",
                              glm::value_ptr(this->AmbientColor));
            ImGui::ColorEdit3("Emissive Color",
                              glm::value_ptr(this->EmissiveColor));
        }
        if(ImGui::CollapsingHeader("Light Settings"))
        {
            LightUBO& data = lightUBO->GetUniformData();
            ImGui::Text("Global values");

            ImGui::ColorEdit4("Fog Color",
                              (glm::value_ptr(data.fogColor)));
            ImGui::ColorEdit4("Global Ambient Color",
                              (glm::value_ptr(data.globalAmbientColor)));
            ImGui::SliderFloat3("Light coefficients",glm::value_ptr(data.coEfficients), 0.0f, 1.0f);
            ImGui::SliderFloat("c1", &data.c1, 0, 1.0);
            ImGui::SliderFloat("c2", &data.c2, 0, 1.0);
            ImGui::SliderFloat("c3", &data.c3, 0, 1.0);
            for(int i = 0; i < MAX_LIGHT; ++i)
            {
                ImGui::PushID(i);
                std::string id = std::string(1, (char)('0' + i));
                if(ImGui::TreeNode(id.c_str(), "Lights %i", i)) {
                    ImGui::ColorEdit4("Light Diffuse Color",
                                      (float *) (glm::value_ptr(data.lightDiffuseColor[i])));
                    ImGui::ColorEdit4("Light Ambient Color",
                                      (float *) (glm::value_ptr(data.lightAmbientColor[i])));
                    ImGui::ColorEdit4("Light Specular Color",
                                      (float *) (glm::value_ptr(data.lightSpecularColor[i])));
                    ImGui::Text("Light type");
                    {
                        static int tempint = 0;
                        tempint = (int)data.lightInfo[i][0];
                        ImGui::RadioButton("Point Light", &tempint, 0);
                        ImGui::RadioButton("Directional", &tempint, 1);
                        ImGui::RadioButton("Spot Light", &tempint, 2);
                        data.lightInfo[i][0] = (float)tempint;
                    }
                    ImGui::Columns(3);
                    ImGui::SliderFloat("Spotlight fall off", &data.lightInfo[i].y, 0.1f, 2 * PI);
                    ImGui::NextColumn();
                    ImGui::SliderFloat("Theta", &data.lightInfo[i].z, 0.01f, 1.0f);
                    ImGui::NextColumn();
                    ImGui::SliderFloat("Phi", &data.lightInfo[i].w, 0.01f, 1.0f);
                    ImGui::Columns(1);
                    ImGui::TreePop();
                }
                ImGui::PopID();
            }
        }
        if(ImGui::CollapsingHeader("Shader Library"))
        {
            ImGui_ShaderLibrary(*this);
        }


    }
    ImGui::End();
    ImGui::Render();
    return Scene::preRender();
}

void SimpleScene_Quad::SetupNanoGUI(GLFWwindow *) {

}

void SimpleScene_Quad::DrawObject(
        const SharedPtr<VertexArray> &vao, const SharedPtr<ProgramPipeline> &pipeline, const SharedPtr<Object> &obj
                                 ) {
    vao->Bind();
    pipeline->GetShader(ProgramPipeline::ShaderType::Vertex)->SetUniform(uniformModel, obj->GetModelMatrix());
    glDrawElements(GL_TRIANGLES, vao->GetIndexSize(), GL_UNSIGNED_INT, nullptr);

}
void SimpleScene_Quad::EditTransform(const Camera& camera, glm::mat4& matrix)
{
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
    if (ImGui::IsKeyPressed(90))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(69))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(82)) // r Key
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(matrix), matrixTranslation, matrixRotation, matrixScale);
    ImGui::InputFloat3("Tr", matrixTranslation, 3);
    ImGui::InputFloat3("Rt", matrixRotation, 3);
    ImGui::InputFloat3("Sc", matrixScale, 3);
    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, glm::value_ptr(matrix));

    if (mCurrentGizmoOperation != ImGuizmo::SCALE)
    {
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
            mCurrentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
            mCurrentGizmoMode = ImGuizmo::WORLD;
    }
    static bool useSnap(false);
    if (ImGui::IsKeyPressed(83))
        useSnap = !useSnap;
    ImGui::Checkbox("", &useSnap);
    ImGui::SameLine();
    glm::vec3 snap;
    switch (mCurrentGizmoOperation)
    {
        case ImGuizmo::TRANSLATE:
            snap = glm::vec3(5.0f, 0.0, 0.0);
            ImGui::InputFloat3("Snap", &snap.x);
            break;
        case ImGuizmo::ROTATE:
            snap = glm::vec3(0.10f, 0.0, 0.0);
            ImGui::InputFloat("Angle Snap", &snap.x);
            break;
        case ImGuizmo::SCALE:
            snap = glm::vec3(1.0f, 0.0, 0.0);
            ImGui::InputFloat("Scale Snap", &snap.x);
            break;
    }
    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    ImGuizmo::Manipulate(glm::value_ptr(camera.GetViewMatrixGLM()), glm::value_ptr(camera.GetPerspectiveGLM()), mCurrentGizmoOperation, mCurrentGizmoMode, glm::value_ptr(matrix), NULL, useSnap ? &snap.x : NULL);
}

void ImGui_DisplayPipeline(SharedPtr<ProgramPipeline>& currentPipe)
{
        auto[vertex, geo, frag] = currentPipe->doShaderExist();
        if (vertex) {
            ImGui::Text("Vertex: %s", std::string(
                    currentPipe->GetShader(ProgramPipeline::ShaderType::Vertex)->GetName()).c_str());
            if (ImGui::Button(("Reload" + std::string(currentPipe->GetShader(
                    ProgramPipeline::ShaderType::Vertex)->GetFile())).c_str())) {
                currentPipe->ReloadShader(ProgramPipeline::ShaderType::Vertex);
            }
        }

        if (frag) {
            ImGui::Text("Fragment: %s", std::string(
                    currentPipe->GetShader(ProgramPipeline::ShaderType::Fragment)->GetName()).c_str());
            if (ImGui::Button(("Reload" + std::string(currentPipe->GetShader(
                    ProgramPipeline::ShaderType::Fragment)->GetFile())).c_str())) {
                currentPipe->ReloadShader(ProgramPipeline::ShaderType::Fragment);
            }
        }

        if (geo) {
            ImGui::Text("Geometry: %s",
                        std::string(currentPipe->GetShader(ProgramPipeline::ShaderType::Geometry)->GetName()).c_str());
            if (ImGui::Button(("Reload" + std::string(currentPipe->GetShader(
                    ProgramPipeline::ShaderType::Geometry)->GetFile())).c_str())) {
                currentPipe->ReloadShader(ProgramPipeline::ShaderType::Geometry);
            }
        }
}

void ImGui_ShaderLibrary(SimpleScene_Quad& scene)
{
    if (ImGui::TreeNode("Pipelines")) {
        // SharedPtr<ProgramPipeline> &currentPipe = scene.PhongLighting;
        if(ImGui::CollapsingHeader("Phong Lighting")) {
            ImGui_DisplayPipeline(scene.PhongLighting);
        }
        if(ImGui::CollapsingHeader("Phong Shading")) {
            ImGui_DisplayPipeline(scene.PhongShading);
        }
        if(ImGui::CollapsingHeader("Blinn-Phong Shading")) {
            ImGui_DisplayPipeline(scene.BlinnShading);
        }
        if(ImGui::CollapsingHeader("Phong Diffuse Shading")) {
            ImGui_DisplayPipeline(scene.PhongDiffuse);
        }
        ImGui::TreePop();
    }
}