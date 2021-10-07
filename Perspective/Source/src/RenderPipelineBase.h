//
// Created by user on 1/26/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: RenderPipelineBase.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/26/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_RENDERPIPELINEBASE_H
#define OPENGLFRAMEWORK_RENDERPIPELINEBASE_H

class Scene;

class Input;

class RenderPipelineBase {
public:
    virtual void Resize(int width, int height) = 0;

    virtual void Render(Scene &scene, Input const &input, float deltaTime, float runningTime) = 0;

protected:
    int width;
    int height;
};


#endif //OPENGLFRAMEWORK_RENDERPIPELINEBASE_H
