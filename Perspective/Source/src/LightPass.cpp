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
#include "GuiSystem.h"

void LightPass::Draw(const LightBuffer &lightBuffer, const GBuffer &gBuffer, Scene &scene) {
    if (!directionalLightProgram) {
        ShaderSystem::getInstance().AddProgram("light/directional", this);
    }

    if (!directionalLightUniformBuffer) {
        glCreateBuffers(1, &directionalLightUniformBuffer);
        glNamedBufferStorage(directionalLightUniformBuffer, sizeof(DirectionalLight), nullptr, GL_DYNAMIC_STORAGE_BIT);
        glBindBufferBase(GL_UNIFORM_BUFFER, PredefinedUniformBlockBinding(DirectionalLightBlock),
                         directionalLightUniformBuffer);
    }

    // Bind the g-buffer
    glBindTextureUnit(0, gBuffer.ambientTexture);
    glBindTextureUnit(1, gBuffer.diffuseTexture);
    glBindTextureUnit(2, gBuffer.specularTexture);
    glBindTextureUnit(3, gBuffer.viewSpacePositionTexture);
    glBindTextureUnit(4, gBuffer.normalTexture);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, lightBuffer.frameBuffer);
    glViewport(0, 0, lightBuffer.width, lightBuffer.height);

    glClearNamedFramebufferfv(lightBuffer.frameBuffer, GL_COLOR, 0, glm::value_ptr(glm::vec4(0.0f)));


    glUseProgram(directionalLightProgram);
    {
        assert(scene.directionalLights.size() == 1);
        auto &dirLight = scene.directionalLights[0];

        dirLight.viewDirection = glm::normalize(scene.mainCamera->GetViewMatrix() * dirLight.worldDirection);
        glNamedBufferSubData(directionalLightUniformBuffer, 0, sizeof(DirectionalLight), &dirLight);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(GL_FUNC_ADD);

        EmptyVAO::Draw();

        glDisable(GL_BLEND);

    }
}

void LightPass::ProgramLoaded(GLuint program) {
    directionalLightProgram = program;

    glProgramUniform1i(directionalLightProgram, PredefinedUniformLocation(u_g_buffer_diffuse), 0);
    glProgramUniform1i(directionalLightProgram, PredefinedUniformLocation(u_g_buffer_ambient), 1);
    glProgramUniform1i(directionalLightProgram, PredefinedUniformLocation(u_g_buffer_specular), 2);
    glProgramUniform1i(directionalLightProgram, PredefinedUniformLocation(u_g_buffer_viewPos), 3);
    glProgramUniform1i(directionalLightProgram, PredefinedUniformLocation(u_g_buffer_normal), 4);
    glProgramUniform1i(directionalLightProgram, PredefinedUniformLocation(u_g_buffer_depth), 5);
}

void LightPass::RenderGui(DirectionalLight &light, LightBuffer const &lightBuffer) {
    if (ImGui::CollapsingHeader("LightPass - ")) {
        ImGui::ColorEdit3("Light Diffuse Color", glm::value_ptr(light.diffuseColor));
        ImGui::ColorEdit3("Light Ambient Color", glm::value_ptr(light.ambientColor));
        ImGui::ColorEdit3("Light Specular Color", glm::value_ptr(light.specularColor));
        GuiSystem::getInstance().Texture(lightBuffer.lightTexture);
    }
}
