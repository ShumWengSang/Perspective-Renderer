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
#include "Animator.h"

#pragma region InternalFunctions
//static int ModelCompareFunc(const void *a, const void *b)
//{
//    auto modelA = (const Model *)a;
//    auto modelB = (const Model *)b;
//
//    if (modelA->material->program <  modelB->material->program) return -1;
//    if (modelA->material->program == modelB->material->program) return  0;
//    if (modelA->material->program >  modelB->material->program) return +1;
//
//    return 0;
//}
#pragma endregion

void GeometryPass::Draw(const GBuffer &gBuffer, Scene &scene) {
    // Frustum data
    std::array<glm::vec4, 6> frustumPlanes{};
    // Shapes::ExtractFrustumPlanes(scene.mainCamera->GetViewProjectionMatrix(), frustumPlanes);
    TransformSystem &transformSystem = TransformSystem::getInstance();

    //static std::vector<Model*> geometryToRender{};
    //geometryToRender.reserve(scene.entities.size());
    //geometryToRender.clear();

/*
	const uint8_t magenta[] = { 255, 0, 255, 255 };
	glClearTexImage(gBuffer.albedoTexture, 0, GL_RGBA, GL_UNSIGNED_BYTE, magenta);
	glClearTexImage(gBuffer.materialTexture, 0, GL_RGBA, GL_UNSIGNED_BYTE, magenta);
*/
//for (const auto& ent : scene.entities)
//{
//    //if(!model.material) continue;
//    //if (!model.material->opaque) continue;

//    geometryToRender.emplace_back(ent.model);
//}

    const float clear[] = {0.0f, 0.0f, 0.0f, 0.0f};
    // Clear the FBO
    glClearNamedFramebufferfv(gBuffer.frameBuffer, GL_COLOR, 0, glm::value_ptr(glm::vec3(0.0f)));
    glClearNamedFramebufferfv(gBuffer.frameBuffer, GL_COLOR, 1, glm::value_ptr(glm::vec3(0.0f)));
    glClearNamedFramebufferfv(gBuffer.frameBuffer, GL_COLOR, 2, glm::value_ptr(glm::vec4(0.0f)));
    glClearNamedFramebufferfv(gBuffer.frameBuffer, GL_COLOR, 3, glm::value_ptr(glm::vec3(0.0f)));
    glClearNamedFramebufferfv(gBuffer.frameBuffer, GL_COLOR, 4, glm::value_ptr(glm::vec3(0.0f)));

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
    // qsort((void *)(geometryToRender.data()), geometryToRender.size(), sizeof(Model), ModelCompareFunc);

    int numDrawCalls = 0;
    int numTriangles = 0;


    GLuint lastProgram = UINT_MAX;
    if (drawObject) {
        for (const auto &ent: scene.entities) {
            Model *model = ent.model;
            Animator *anim = ent.animator;
            GLuint program = model->material->program;

            if (program == 0) {
                continue;
            }

            if (program != lastProgram) {
                glUseProgram(program);
                lastProgram = program;
            }

            Transform &transform = transformSystem.Get(model->transformID);
            const Transform &prevTransform =
                    transformSystem.GetPrevious(model->transformID);
            model->material->BindUniforms(transform, prevTransform);

            if (anim != nullptr) {
                const auto &transforms = anim->GetFinalBoneMatrices();
                // Convert to mat4
                std::vector<glm::mat4> matrices(transforms.size());
                for (int i = 0; i < transforms.size(); ++i) {
                    matrices[i] = transforms[i].ToMat4();
                }
                glUniformMatrix4fv(glGetUniformLocation(model->material->program, "u_finalBonesMatrices"),
                                   transforms.size(), GL_FALSE, glm::value_ptr(matrices[0]));
            }

            if (model->material->cullBackface)
                glEnable(GL_CULL_FACE);
            else
                glDisable(GL_CULL_FACE);

            model->Draw();

            numDrawCalls += 1;
            numTriangles += model->TriangleCount();
        }
    }

    glDepthMask(true);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void GeometryPass::ProgramLoaded(GLuint program) {
    depthOnlyProgram = program;
    modelMatrixLocation = glGetUniformLocation(depthOnlyProgram, "u_world_from_local");
}
