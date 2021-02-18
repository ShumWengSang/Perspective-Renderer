//
// Created by user on 1/26/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: GBuffer.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/26/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "GBuffer.h"
#include "TextureSystem.h"
#include "ShaderLocations.h"
#include "Logger.h"
#include "GuiSystem.h"

void GBuffer::RecreateGpuResources(int width, int height) {
    this->width = width;
    this->height = height;

    glDeleteTextures(1, &diffuseTexture);
    glDeleteTextures(1, &ambientTexture);
    glDeleteTextures(1, &specularTexture);
    glDeleteTextures(1, &viewSpacePositionTexture);
    glDeleteTextures(1, &normalTexture);
    glDeleteTextures(1, &depthTexture);

    TextureSystem& textureSystem = TextureSystem::getInstance();

    diffuseTexture = textureSystem.CreateTexture(width, height, GL_RGB16F, GL_NEAREST, GL_NEAREST, false);
    ambientTexture = textureSystem.CreateTexture(width, height, GL_RGBA8, GL_NEAREST, GL_NEAREST, false);
    specularTexture = textureSystem.CreateTexture(width, height, GL_RGBA16F, GL_NEAREST, GL_NEAREST, false);
    viewSpacePositionTexture = textureSystem.CreateTexture(width, height, GL_RGB16F, GL_NEAREST, GL_NEAREST, false);
    normalTexture = textureSystem.CreateTexture(width, height, GL_RGB16F, GL_NEAREST, GL_NEAREST, false);
    depthTexture = textureSystem.CreateTexture(width, height, GL_DEPTH_COMPONENT32F, GL_NEAREST, GL_NEAREST, false);

    // Setup the swizzle for the depth textures so all color channels are depth
    GLenum depthSwizzle[] = { GL_RED, GL_RED, GL_RED, GL_ALPHA };
    glTextureParameteriv(depthTexture, GL_TEXTURE_SWIZZLE_RGBA, (GLint *)depthSwizzle);

    GLenum drawBuffers[] = {
            PredefinedOutputLocation(o_g_buffer_diffuse),
            PredefinedOutputLocation(o_g_buffer_ambient),
            PredefinedOutputLocation(o_g_buffer_specular),
            PredefinedOutputLocation(o_g_buffer_viewPos),
            PredefinedOutputLocation(o_g_buffer_normal)
    };
    if (!frameBuffer)
    {
        glCreateFramebuffers(1, &frameBuffer);

        int numDrawBuffers = sizeof(drawBuffers) / sizeof(GLenum);
        glNamedFramebufferDrawBuffers(frameBuffer, numDrawBuffers, drawBuffers);
    }

    GLenum attachment = PredefinedOutputLocation(o_g_buffer_diffuse);
    glNamedFramebufferTexture(frameBuffer, attachment, diffuseTexture, 0);
    attachment = PredefinedOutputLocation(o_g_buffer_ambient);
    glNamedFramebufferTexture(frameBuffer, attachment, ambientTexture, 0);
    attachment = PredefinedOutputLocation(o_g_buffer_specular);
    glNamedFramebufferTexture(frameBuffer, attachment, specularTexture, 0);
    attachment = PredefinedOutputLocation(o_g_buffer_viewPos);
    glNamedFramebufferTexture(frameBuffer, attachment, viewSpacePositionTexture, 0);
    attachment = PredefinedOutputLocation(o_g_buffer_normal);
    glNamedFramebufferTexture(frameBuffer, attachment, normalTexture, 0);

    // Depth
    glNamedFramebufferTexture(frameBuffer, GL_DEPTH_ATTACHMENT, depthTexture, 0);

    GLenum status = glCheckNamedFramebufferStatus(frameBuffer, GL_DRAW_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        LogError("The G-buffer framebuffer is not complete!");
    }
}

void GBuffer::RenderToDebugTextures() const {
    Log("Deprecated");
}

void GBuffer::RenderGui(const std::string &message) const {
    if (ImGui::CollapsingHeader(("G-Buffer - " + message).c_str()))
    {
        GuiSystem& guiSystem = GuiSystem::getInstance();
        ImGui::Text("Diffuse:");
        guiSystem.Texture(diffuseTexture);
        ImGui::Text("View Space Position:");
        guiSystem.Texture(viewSpacePositionTexture);
        ImGui::Text("Normals [-1, 1]: ");
        guiSystem.Texture(normalTexture);
        ImGui::Text("Depth:");
        guiSystem.Texture(depthTexture);
    }
}
