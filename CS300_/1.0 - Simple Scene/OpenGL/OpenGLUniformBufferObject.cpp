//
// Created by user on 11/3/2020.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: OpenGLUniformBufferObject.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Simple_Scene
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 11/3/2020
 * End Header --------------------------------------------------------*/
#include "OpenGLUniformBufferObject.h"
#include "OpenGLShader.h"

void OpenGLUniformBufferObject::SetData(const void *data, unsigned int size, unsigned int offset) {

}

void OpenGLUniformBufferObject::SetBlockBinding(const SharedPtr<Shader> &shader) {
    shader->SetUniformBuffer(*this);
}

void OpenGLUniformBufferObject::Unbind() const {
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLUniformBufferObject::Bind() const {
    glBindBuffer(GL_UNIFORM_BUFFER, id);
}

OpenGLUniformBufferObject::OpenGLUniformBufferObject(std::string const &name, size_t size) : Name(name) {
    glGenBuffers(1, &id);
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
}

OpenGLUniformBufferObject::~OpenGLUniformBufferObject() {
    glDeleteBuffers(1, &id);
}

const std::string &OpenGLUniformBufferObject::GetName() const {
    return Name;
}
