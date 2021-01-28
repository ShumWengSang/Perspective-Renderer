//
// Created by user on 1/26/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: LightPass.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/26/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "LightPass.h"
#include "LightBuffer.h"
#include "GBuffer.h"
#include "Scene.h"
#include "ShaderSystem.h"
#include "ShaderLocations.h"
#include "CameraBase.h"
#include "EmptyVAO.h"
#include "DirectionalLight.h"
#include "GuiSystem.h"

void LightPass::Draw(const LightBuffer &lightBuffer, const GBuffer &gBuffer, Scene &scene) {
    if (!directionalLightProgram)
    {
        ShaderSystem::getInstance().AddProgram("light/directional", this);
    }

    if (!directionalLightUniformBuffer)
    {
        glCreateBuffers(1, &directionalLightUniformBuffer);
        glNamedBufferStorage(directionalLightUniformBuffer, sizeof(DirectionalLight), nullptr, GL_DYNAMIC_STORAGE_BIT);
        glBindBufferBase(GL_UNIFORM_BUFFER, PredefinedUniformBlockBinding(DirectionalLightBlock), directionalLightUniformBuffer);
    }

    // Bind the g-buffer
    glBindTextureUnit(0, gBuffer.albedoTexture);
    glBindTextureUnit(1, gBuffer.materialTexture);
    glBindTextureUnit(2, gBuffer.normalTexture);
    glBindTextureUnit(3, gBuffer.depthTexture);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, lightBuffer.frameBuffer);
    glViewport(0, 0, lightBuffer.width, lightBuffer.height);

    glUseProgram(directionalLightProgram);
    {
        assert(scene.directionalLights.size() == 1);
        auto &dirLight = scene.directionalLights[0];

        dirLight.viewDirection = scene.mainCamera->GetViewMatrix() * dirLight.worldDirection;
        glNamedBufferSubData(directionalLightUniformBuffer, 0, sizeof(DirectionalLight), &dirLight);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(GL_FUNC_ADD);

        glDisable(GL_DEPTH_TEST);

        EmptyVAO::Draw();

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

    }
    if (ImGui::CollapsingHeader("Light pass"))
    {
        ImGui::SliderFloat("Sun intensity", &scene.directionalLights[0].color.a, 0.0f, 1.0f);
        GuiSystem::getInstance().Texture(lightBuffer.lightTexture);
    }
}

void LightPass::ProgramLoaded(GLuint program) {
    directionalLightProgram = program;

    glProgramUniform1i(directionalLightProgram, PredefinedUniformLocation(u_g_buffer_albedo), 0);
    glProgramUniform1i(directionalLightProgram, PredefinedUniformLocation(u_g_buffer_material), 1);
    glProgramUniform1i(directionalLightProgram, PredefinedUniformLocation(u_g_buffer_norm), 2);
    glProgramUniform1i(directionalLightProgram, PredefinedUniformLocation(u_g_buffer_depth), 3);
}
