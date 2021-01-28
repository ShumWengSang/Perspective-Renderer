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

void FinalPass<AssignmentOne>::Draw(const LightBuffer &lightBuffer, Scene &scene) {
    static bool once = false;
    if(!once)
    {
        ShaderSystem::getInstance().AddProgram("quad.vert.glsl", "final.frag.glsl", this);
        once = true;
    }
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glViewport(0, 0, lightBuffer.width, lightBuffer.height);

    glUseProgram(finalProgram);
    {
        EmptyVAO::Draw();
    }

    glEnable(GL_DEPTH_TEST);

}

void FinalPass<AssignmentOne>::ProgramLoaded(GLuint program) {
    if(finalProgram && program == finalProgram)
    {
        glProgramUniform1i(finalProgram, PredefinedUniformLocation(u_texture), 0);
    }
}
