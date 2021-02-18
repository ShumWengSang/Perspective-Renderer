//
// Created by user on 1/27/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: LightBuffer.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/27/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "LightBuffer.h"
#include "TextureSystem.h"
#include "ShaderLocations.h"
#include "GBuffer.h"
#include "Logger.h"

void LightBuffer::RecreateGpuResources(int width, int height, const GBuffer &gBuffer) {
    this->width = width;
    this->height = height;

    TextureSystem& textureSystem = TextureSystem::getInstance();

    glDeleteTextures(1, &lightTexture);
    lightTexture = textureSystem.CreateTexture(width, height, GL_RGBA32F, GL_NEAREST, GL_NEAREST);

    if (!frameBuffer)
    {
        glCreateFramebuffers(1, &frameBuffer);
        glNamedFramebufferDrawBuffer(frameBuffer, PredefinedOutputLocation(o_color));
    }

    glNamedFramebufferTexture(frameBuffer, PredefinedOutputLocation(o_color), lightTexture, 0);

    // Use the g-buffer depth as the depth in the light pass (for reading only)
    // glNamedFramebufferTexture(frameBuffer, GL_DEPTH_ATTACHMENT, gBuffer.depthTexture, 0);

    GLenum status = glCheckNamedFramebufferStatus(frameBuffer, GL_DRAW_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        Log("The Light buffer framebuffer is not complete!");
    }

}
