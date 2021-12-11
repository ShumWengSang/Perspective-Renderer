//
// Created by user on 1/27/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: FinalPass.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/27/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "FinalPass.h"
#include "ShaderSystem.h"
#include "ShaderLocations.h"
#include "LightBuffer.h"
#include "EmptyVAO.h"
#include "Scene.h"
#include "GBuffer.h"

void FinalPass<AssignmentOne>::Draw(const GBuffer &gBuffer, const LightBuffer &lightBuffer, const Scene &scene) {
    static bool once = false;
    if (!once) {
        ShaderSystem::getInstance().AddProgram("quad.vert.glsl", "final.frag.glsl", this);
        once = true;
    }
    glDisable(GL_BLEND);
    //glDisable(GL_DEPTH_TEST);

    glBindTextureUnit(0, lightBuffer.lightTexture);
    glBindTextureUnit(1, scene.probe.skyCube);
    glBindTextureUnit(2, gBuffer.depthTexture);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glViewport(0, 0, lightBuffer.width, lightBuffer.height);

    glDepthFunc(GL_ALWAYS);
    glUseProgram(finalProgram);
    {
        this->copyDepthBuffer.UpdateUniformIfNeeded(finalProgram);
        EmptyVAO::Draw();
    }
    glDepthFunc(GL_LEQUAL);
    //glEnable(GL_DEPTH_TEST);
    // ImGui::Checkbox("Copy Depth Buffer", (bool *) (&copyDepthBuffer.value));
/*    if(copyDepthBuffer.value)
    {
        glBlitNamedFramebuffer(gBuffer.frameBuffer, 0, 0, 0, gBuffer.width, gBuffer.height, 0, 0, gBuffer.width, gBuffer.height,
                               GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }*/
}

void FinalPass<AssignmentOne>::ProgramLoaded(GLuint program) {
    finalProgram = program;
    if (finalProgram && program == finalProgram) {
        glProgramUniform1i(finalProgram, PredefinedUniformLocation(u_texture), 0);
        glProgramUniform1i(finalProgram, PredefinedUniformLocation(u_textureCube), 1);
        glProgramUniform1i(finalProgram, PredefinedUniformLocation(u_g_buffer_depth), 2);
    }
}
