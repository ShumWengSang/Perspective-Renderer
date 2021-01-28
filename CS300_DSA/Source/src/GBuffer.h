//
// Created by user on 1/26/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: GBuffer.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/26/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_GBUFFER_H
#define OPENGLFRAMEWORK_GBUFFER_H


class GBuffer {
public:
    int width;
    int height;

    GLuint frameBuffer;

    //RGBA8: RGB = Diffuse
    GLuint albedoTexture = 0;

    // RGBA8: RGB - Specular, A = Shininess
    GLuint materialTexture = 0;

    // RGBA8: RGB - normal,
    GLuint normalTexture = 0;

    // R24f: projected non-linear depth
    GLuint depthTexture = 0;

    // Create or recreate the g-buffer textures with the given dimensions
    void RecreateGpuResources(int width, int height);

    void RenderToDebugTextures() const;

    void RenderGui(const std::string& message) const;
};


#endif //OPENGLFRAMEWORK_GBUFFER_H
