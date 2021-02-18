//
// Created by user on 1/26/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: CameraBase.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/26/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "ShaderLocations.h"
#include "CameraBase.h"
#include "App.h"


void CameraBase::DrawEditorGui() {

}

void CameraBase::CommitToGpu() {
    static bool once = false;

    if(!once)
    {
        cameraBuffer.BindBufferBase(BufferObjectType::Uniform, PredefinedUniformBlockBinding(CameraUniformBlock));
        once = true;
    }
    cameraBuffer.memory.ViewFromWorldMatrix = viewFromWorld;
    cameraBuffer.memory.ProjectFromViewMatrix = projectionFromView;
    cameraBuffer.memory.camPos = glm::vec4(this->position,1.0);
    float width = App::GetApp().windowWidth;
    float height = App::GetApp().windowHeight;

    glm::vec4 nearFar = glm::vec4(zNear, zFar, fieldOfView, width/ height);
    cameraBuffer.memory.nearFar = nearFar;

    cameraBuffer.UpdateGpuBuffer();
}


void CameraBase::LookAt(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up) {
    this->position = position;
    auto direction = glm::normalize(target - position);
    this->orientation = glm::quatLookAtLH(direction, up);
}

void CameraBase::Resize(int width, int height) {
    targetPixelsWidth = width;
    targetPixelsHeight = height;
}
