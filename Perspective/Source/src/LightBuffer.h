//
// Created by user on 1/27/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: LightBuffer.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/27/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_LIGHTBUFFER_H
#define OPENGLFRAMEWORK_LIGHTBUFFER_H

class GBuffer;

// Also known as P-buffer, or pixelBuffer
class LightBuffer {
public:
    int width, height;

    GLuint frameBuffer;

    // GL_RGB8 - Accumulated light
    GLuint lightTexture = 0;

    void RecreateGpuResources(int width, int height, GBuffer const &gBuffer);
};


#endif //OPENGLFRAMEWORK_LIGHTBUFFER_H
