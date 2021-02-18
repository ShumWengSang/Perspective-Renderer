//
// Created by user on 1/31/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: DebugSystem.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/31/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_DEBUGSYSTEM_H
#define OPENGLFRAMEWORK_DEBUGSYSTEM_H

class DebugModel;
// Graphical debug.
class DebugSystem {
public:
    enum DebugType : int
    {
        Face_Normal = 0,
        Vertex_Normal = 1,
        All
    };
    void AddDebugModel(DebugType type, DebugModel const & model);
    const std::unordered_map<DebugType, std::vector<DebugModel>>& GetDebugModels() const;
private:
    std::unordered_map<DebugType, std::vector<DebugModel>> debugModels;
    friend class ForwardRendering;
};


#endif //OPENGLFRAMEWORK_DEBUGSYSTEM_H
