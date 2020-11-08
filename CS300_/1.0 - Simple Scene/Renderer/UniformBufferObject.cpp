//
// Created by user on 11/3/2020.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: UniformBufferObject.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Simple_Scene
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 11/3/2020
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "UniformBufferObject.h"

SharedPtr<UniformBufferObject> UniformBufferObject::Create(std::string const &name, size_t size) {
    return CreateSharedPtr<UniformBufferObject>(name, size);
}
