//
// Created by user on 1/26/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: FPSCamera.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/26/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_FPSCAMERA_H
#define OPENGLFRAMEWORK_FPSCAMERA_H

#include "CameraBase.h"

class FpsCamera : public CameraBase {
public:
    FpsCamera() = default;

    ~FpsCamera() = default;

    void Update(const Input &input, float dt) override;

private:
    glm::vec3 velocity{};

    const float maxSpeed = 10.0f;
    const float timeToMaxSpeed = 0.25f;
    const float timeFromMaxSpeed = 0.60f;
    const float stopThreshold = 0.02f;

    //

    glm::vec3 pitchYawRoll{0.0f};

    float rotationMultiplier = 30.0f;
    float rotationDampening = 0.000005f;

    glm::quat bankingOrientation{1, 0, 0, 0};
    const float baselineBankAngle{glm::radians(30.0f)};

    //

    float fieldOfView{glm::radians(60.0f)};
    float targetFieldOfView{fieldOfView};

    const float zoomSensitivity{0.15f};
    const float minFieldOfView{glm::radians(15.0f)};
    const float maxFieldOfView{glm::radians(60.0f)};
};


#endif //OPENGLFRAMEWORK_FPSCAMERA_H
