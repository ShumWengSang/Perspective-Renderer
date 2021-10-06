//
// Created by user on 1/30/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: ForwardRendering.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/30/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "ForwardRendering.h"
#include "ShaderSystem.h"
#include "TransformSystem.h"
#include "Model.h"
#include "DebugModel.h"
#include "Scene.h"
#include "DebugSystem.h"
#include "Animator.h"

void ForwardRendering::Draw(const Scene &scene) {
    static bool once = false;
    if (!once) {
        ShaderSystem::getInstance().AddProgram("material/debugLine", this);
        cylinder =  new Model("Common/cylinder.fbx");
        once = true;
    }
    TransformSystem & transformSystem = TransformSystem::getInstance();
    static bool debugOptions[DebugSystem::DebugType::All] = {false, false};

    //const auto& debugModels = scene.debugSystem.GetDebugModels();
    //for(const auto& debugModelPair : debugModels)
    //{
    //    if(debugOptions[debugModelPair.first])
    //    {
    //        for (const DebugModel &model : debugModelPair.second) {
    //            glUseProgram(lineProgram);
    //            this->debugLineMaterial.BindUniforms(transformSystem.Get(model.transformID).matrix);
    //            model.Draw();
    //        }
    //    }
    //}
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    Transform& trans = TransformSystem::getInstance().Get(scene.entities[0].model->transformID);
	glUseProgram(lineProgram);
	{
        auto finalMatrices = scene.entities[0].animator->DrawBones(trans.matrix);
        for (int i = 0; i < finalMatrices.size(); ++i)
        {
            this->debugLineMaterial.BindUniforms(finalMatrices[i]);
            if (i == 0)
            {
                
            }
            cylinder->Draw();
        }
	}

    ImGui::Checkbox("Enable Face Normal Lines", &debugOptions[0]);
    ImGui::Checkbox("Enable Vertex Normal Lines", &debugOptions[1]);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void ForwardRendering::ProgramLoaded(GLuint program) {
    this->lineProgram = program;
    this->u_color = glGetUniformLocation(program, "u_color");
}
