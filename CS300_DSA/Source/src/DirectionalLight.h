//
// Created by user on 1/27/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: DirectionalLight.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/27/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_DIRECTIONALLIGHT_H
#define OPENGLFRAMEWORK_DIRECTIONALLIGHT_H


class DirectionalLight {
public:
    glm::vec4 worldDirection;
    glm::vec4 viewDirection;

    glm::vec4 color;
};


#endif //OPENGLFRAMEWORK_DIRECTIONALLIGHT_H
