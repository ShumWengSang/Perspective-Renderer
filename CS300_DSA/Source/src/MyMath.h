//
// Created by user on 1/30/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: Math.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/30/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_MYMATH_H
#define OPENGLFRAMEWORK_MYMATH_H


namespace MyMath {

    float Remap(float value, float low1, float high1, float low2, float high2);
    glm::vec4 Remap(const glm::vec4 &value, const glm::vec2 &inMinMax, const glm::vec2 &outMinMax);
};


#endif //OPENGLFRAMEWORK_MYMATH_H
