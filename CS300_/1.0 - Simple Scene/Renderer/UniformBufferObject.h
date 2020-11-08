//
// Created by user on 11/3/2020.
//

//
// Created by user on 11/3/2020.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: UniformBufferObject.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Simple_Scene
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 11/3/2020
 * End Header --------------------------------------------------------*/

#ifndef SIMPLE_SCENE_UNIFORMBUFFEROBJECT_H
#define SIMPLE_SCENE_UNIFORMBUFFEROBJECT_H
#include "../stdafx.h"
#include "UniformBufferObject.h"
class Shader;
class UniformBufferObject {
public:
    virtual ~UniformBufferObject() = default;
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual void SetBlockBinding(SharedPtr<Shader> const & shader) = 0;
    virtual void SetData(void const * data, unsigned size, unsigned offset) = 0;

    virtual std::string const & GetName() const = 0;
    static SharedPtr<UniformBufferObject> Create(std::string const &name, size_t size);
};


#endif //SIMPLE_SCENE_UNIFORMBUFFEROBJECT_H
