//
// Created by user on 1/26/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: CameraBase.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/26/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_CAMERABASE_H
#define OPENGLFRAMEWORK_CAMERABASE_H

#include "BufferObject.hpp"
#include "CameraUniforms.h"


class Input;

class CameraBase {
public:
    CameraBase() = default;

    virtual ~CameraBase() = default;

    void DrawEditorGui();

    void CommitToGpu();

    void LookAt(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up = {0, 1, 0});

    void Resize(int width, int height);

    virtual void Update(const Input &input, float dt) = 0;

    const glm::vec3 &GetPosition() const { return position; }

    const glm::vec3 &GetForward() const { return forward; }

    const glm::vec3 &GetLookAt() const { return lookAt; }

    const glm::quat &GetOrientation() const { return orientation; }

    const glm::mat4 &GetViewMatrix() const { return viewFromWorld; }

    const glm::mat4 &GetProjectionMatrix() const { return projectionFromView; }

    glm::mat4 GetViewProjectionMatrix() const { return GetProjectionMatrix() * GetViewMatrix(); }

protected:
    BufferObject<CameraUniforms> cameraBuffer;
    glm::mat4 viewFromWorld;
    glm::mat4 projectionFromView;
    glm::quat orientation{};
    glm::vec3 position{};
    glm::vec3 forward{};
    glm::vec3 lookAt{};
    float fieldOfView{3.141562f / 3.0f};
    float zNear{0.001f};
    float zFar{250000.0f};
    int targetPixelsWidth;
    int targetPixelsHeight;
};


#endif //OPENGLFRAMEWORK_CAMERABASE_H
