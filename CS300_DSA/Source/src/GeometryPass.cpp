//
// Created by user on 1/27/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: GeometryPass.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/27/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "GeometryPass.h"
#include "GBuffer.h"
#include "Scene.h"
#include "CameraBase.h"
#include "Shapes.h"
#include "Model.h"
#include "Material.h"
#include "TransformSystem.h"
#include "ShaderLocations.h"

#pragma region InternalFunctions
static int ModelCompareFunc(const void *a, const void *b)
{
    auto modelA = (const Model *)a;
    auto modelB = (const Model *)b;

    if (modelA->material->program <  modelB->material->program) return -1;
    if (modelA->material->program == modelB->material->program) return  0;
    if (modelA->material->program >  modelB->material->program) return +1;

    return 0;
}
#pragma endregion

void GeometryPass::Draw(const GBuffer &gBuffer, Scene &scene) {
    // Frustum data
    std::array<glm::vec4, 6> frustumPlanes{};
    Shapes::ExtractFrustumPlanes(scene.mainCamera->GetViewProjectionMatrix(), frustumPlanes);
    TransformSystem& transformSystem = TransformSystem::getInstance();

    static std::vector<Model> geometryToRender{};
    geometryToRender.reserve(scene.models.size());
    geometryToRender.clear();

/*
    const uint8_t magenta[] = { 255, 0, 255, 255 };
    glClearTexImage(gBuffer.albedoTexture, 0, GL_RGBA, GL_UNSIGNED_BYTE, magenta);
    glClearTexImage(gBuffer.materialTexture, 0, GL_RGBA, GL_UNSIGNED_BYTE, magenta);
*/
    for (const Model& model : scene.models)
    {
        if(!model.material) continue;
        if (!model.material->opaque) continue;

        auto transform = transformSystem.Get(model.transformID);
        Shapes::BoundingSphere worldSpaceBounds = model.bounds;
        worldSpaceBounds.center += transform.position;
        worldSpaceBounds.radius *= Shapes::VectorMaxComponent(transform.scale); // TODO: We can't use max-component, rather we would need the largest singular value!
        if (!InsideFrustum(frustumPlanes, worldSpaceBounds)) continue;

        geometryToRender.emplace_back(model);
    }

    const float clear[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    glClearTexImage(gBuffer.normalTexture, 0, GL_RGBA, GL_UNSIGNED_BYTE, clear);
    glClearTexImage(gBuffer.albedoTexture, 0, GL_RGBA, GL_UNSIGNED_BYTE, clear);
    glClearTexImage(gBuffer.materialTexture, 0, GL_RGBA, GL_UNSIGNED_BYTE, clear);

    const float farDepth = 1.0f;
    glClearTexImage(gBuffer.depthTexture, 0, GL_DEPTH_COMPONENT, GL_FLOAT, &farDepth);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer.frameBuffer);
    glViewport(0, 0, gBuffer.width, gBuffer.height);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glPolygonMode(GL_FRONT_AND_BACK, wireframeRendering ? GL_LINE : GL_FILL);

    // Sort geometry so that we can optimize the number of shader program switches, i.e. calling glUseProgram
    qsort((void *)(geometryToRender.data()), geometryToRender.size(), sizeof(Model), ModelCompareFunc);

    int numDrawCalls = 0;
    int numTriangles = 0;



    GLuint lastProgram = UINT_MAX;
    for (const Model& model : geometryToRender)
    {
        GLuint program = model.material->program;

        if (program == 0)
        {
            continue;
        }

        if (program != lastProgram)
        {
            glUseProgram(program);
            lastProgram = program;
        }

        Transform& transform = transformSystem.Get(model.transformID);
        const Transform& prevTransform = transformSystem.GetPrevious(model.transformID);
        model.material->BindUniforms(transform, prevTransform);

        if (model.material->cullBackface) glEnable(GL_CULL_FACE);
        else glDisable(GL_CULL_FACE);

        model.Draw();

        numDrawCalls += 1;
        numTriangles += model.TriangleCount();
    }

    if(ImGui::CollapsingHeader("Geometry Pass"))
    {
        ImGui::Checkbox("Draw wireframes", &wireframeRendering);
        ImGui::Text("Draw calls: %d", numDrawCalls);
        ImGui::Text("Triangles:  %d", numTriangles);
    }
}

void GeometryPass::ProgramLoaded(GLuint program) {
    depthOnlyProgram = program;
    modelMatrixLocation = glGetUniformLocation(depthOnlyProgram, "u_world_from_local");
}
