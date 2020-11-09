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


static const glm::vec4 O  = Math::point(0,0,0),
                       EX = Math::vector(1,0,0),
                       EY = Math::vector(0,1,0),
                       EZ = Math::vector(0,0,1);


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
                                                                                O + 10.0f * EZ, -EZ, EY, 45.0f, windowWidth / (float)windowHeight, 0.1f, 1000.0f
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
    // Check if mesh was loaded
    if(!mesh)
    {
        std::cout << "Error! " << FullPath << " not loaded!" << std::endl;
    }
    if(!planeMesh)
    {
        std::cout << "Error! Quad.obj not loaded!" << std::endl;
    }
    planeMesh->get()->GenerateVertexNormals().GenerateColors({0.1f,0.1f,0.1f}).GenerateTexCoords(texture_mode);
    // We need to generate the normals and the colors
    mesh->get()->GenerateVertexNormals().GenerateColors({0.4f, 0.3f, 0.1f}).GenerateTexCoords(texture_mode);
    sphereMesh->get()->GenerateColors({0.3f, 0.7f, 0.9f}).GenerateTexCoords(texture_mode);

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
    }
    // Create VAO for plane object
    {
        std::vector<Vertex> QuadVertices = Mesh::CreateVertexFromMesh(*planeMesh.value());
        auto const[vao_mesh, vbo_mesh, ibo_mesh] = createGeometry(QuadVertices, planeMesh->get()->Index, vertexFormat);
        planeArray = CreateSharedPtr<VertexArray>(vao_mesh);
        planeArray->PushBuffer({vbo_mesh, ibo_mesh});
        planeArray->SetIndexSize(planeMesh->get()->Index.size());
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

}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SimpleScene_Quad::Init()
{
    SetupBuffers();

    meshObj = CreateSharedPtr<Object>();
    meshObj->SetUpdate([&](Object& obj)
                       {
                           glm::vec3 scaleVector = glm::vec3(1.0f);
                           glm::vec3 centroid = meshPosition;
                           obj.SetModelMatrix(glm::translate( centroid ) *
                                      glm::rotate(cubeAngle, glm::vec3(0.0f, 1.0f, 0.0f)) *
                                      glm::scale( scaleVector ));
                       }); // angleOfRotation

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
                                    glm::vec3 centroid = glm::vec3( 0,0,3);
                                    obj.SetModelMatrix(glm::scale( scaleVector ) *
                                               glm::rotate(angleOfRotation + (2 * PI / 16) * i + PI, glm::vec3(0.0f, 1.0f, 0.0f)) *
                                               glm::translate( centroid ));
                                });
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
        lightUbo.lightPos[i] = glm::column(sphereObj[i]->GetModelMatrix(), 3);
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
    }


    {
        PhongDiffuse->Bind();
        auto vertexProgram = PhongDiffuse->GetShader(ProgramPipeline::ShaderType::Vertex);
        auto fragProgram = PhongDiffuse->GetShader(ProgramPipeline::ShaderType::Fragment);
        // Render the diffuse only pass.

        // Update the uniforms for the diffuse shader
        fragProgram->SetUniform(uniformLightDirection, lightDirection);
        fragProgram->SetUniform(uniformlightColor, lightColor);

        // Draw spheres
        for (int i = 0; i < 1; i++) {
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

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    if (ImGui::Begin("Assignment 1"))
    {
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
        if (ImGui::CollapsingHeader("Background color")) {
            ImGui::ColorEdit3("Background Color",
                              (float *) (&(this->backgroundColor)));
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
            ImGui::SliderFloat("Cube Rotation", &cubeAngle, 0.0f, 2 * PI);

            ImGui::DragFloat3("Mesh position", glm::value_ptr(meshPosition));
            // meshPosition
        }
        if(ImGui::CollapsingHeader("Light Settings"))
        {
            LightUBO& data = lightUBO->GetUniformData();
            for(int i = 0; i < MAX_LIGHT; ++i)
            {
                std::string id = std::string(1, (char)('0' + i));
                if(ImGui::TreeNode(id.c_str(), "Lights %i", i)) {
                    ImGui::ColorEdit4("Light Color",
                                      (float *) (glm::value_ptr(data.lightColor[i])));
                    ImGui::Text("Light type");
                    {
                        ImGui::RadioButton("Point Light", (int *) &data.lightInfo[i][0], 0);
                        ImGui::RadioButton("Directional", (int *) &data.lightInfo[i][0], 1);
                        ImGui::RadioButton("Spot Light", (int *) &data.lightInfo[i][0], 2);
                    }
                    ImGui::DragFloat("Spotlight fall off", &data.lightInfo[i][1]);
                    ImGui::SameLine();
                    ImGui::DragFloat("Theta", &data.lightInfo[i][2]);
                    ImGui::SameLine();
                    ImGui::DragFloat("Phi", &data.lightInfo[i][3]);
                    ImGui::TreePop();
                }
            }
            ImGui::Text("Global values");
            ImGui::ColorEdit4("Fog Color",
                              (glm::value_ptr(data.fogColor)));
            ImGui::ColorEdit4("Global Ambient Color",
                             (glm::value_ptr(data.globalAmbientColor)));
            ImGui::SliderFloat3("Light coefficients",glm::value_ptr(data.coEfficients), 0.0f, 1.0f);
            ImGui::SliderFloat("c1", &data.c1, 0, 1.0);
            ImGui::SliderFloat("c2", &data.c2, 0, 1.0);
            ImGui::SliderFloat("c3", &data.c3, 0, 1.0);
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
        ImGui::TreePop();
    }
}