//
// Created by user on 1/31/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: DebugSystem.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/31/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "DebugSystem.h"
#include "DebugModel.h"

#


void DebugSystem::AddDebugModel(DebugSystem::DebugType type, const DebugModel &model) {
    debugModels[type].emplace_back(model);
}

const std::unordered_map<DebugSystem::DebugType, std::vector<DebugModel>> &DebugSystem::GetDebugModels() const {
    return debugModels;
}
