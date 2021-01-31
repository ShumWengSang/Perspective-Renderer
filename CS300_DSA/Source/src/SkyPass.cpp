//
// Created by user on 1/30/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: SkyPass.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/30/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "SkyPass.h"
#include "ShaderLocations.h"
#include "LightBuffer.h"
#include "GBuffer.h"
#include "Scene.h"
#include "ShaderSystem.h"
#include "EmptyVAO.h"
#include "SkyProbe.h"
#include "Logger.h"

void SkyPass::Draw(const LightBuffer &lightBuffer, const GBuffer &gBuffer, Scene &scene) {
    LogError("This is deprecated");
/*    // Load the sky box shader
    if(!skyProgram)
    {
        ShaderSystem::getInstance().AddProgram("Skybox", this);

        glCreateFramebuffers(1, &framebuffer);
        GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
        glNamedFramebufferDrawBuffers(framebuffer, 3, drawBuffers);
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_EQUAL);
    glDepthMask(GL_FALSE);

    // TODO: Maybe don't rebind all textures every frame?
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
    glNamedFramebufferTexture(framebuffer, PredefinedOutputLocation(o_color), lightBuffer.lightTexture, 0);
    glViewport(0, 0, lightBuffer.width, lightBuffer.height);

    glUseProgram(skyProgram);
    glBindTextureUnit(0, scene.probe.skyCube);
    EmptyVAO::Draw();

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);*/
}

void SkyPass::ProgramLoaded(GLuint program) {
    this->skyProgram = program;
    glProgramUniform1i(skyProgram, PredefinedUniformLocation(u_texture), 0);
}
