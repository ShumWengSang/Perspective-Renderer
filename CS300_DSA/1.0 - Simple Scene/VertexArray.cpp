//
// Created by user on 11/5/2020.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: VertexArray.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Simple_Scene
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 11/5/2020
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "VertexArray.h"

VertexArray::VertexArray(GLuint id) : ID(id){

}

VertexArray::~VertexArray() {
    DestroyVA();
}

GLuint VertexArray::GetID() {
    return ID;
}

void VertexArray::SetID(GLuint ID) {
    DestroyVA();
    ID = ID;
}

void VertexArray::DestroyVA() {
    for(size_t i = 0; i < Buffers.size(); ++i)
    {
        glDeleteBuffers(1, &Buffers[i]);
    }
    glDeleteVertexArrays(1, &ID);
}

VertexArray &VertexArray::PushBuffer(std::initializer_list<GLuint> buffers) {
    Buffers.insert(Buffers.begin(), buffers);
    return *this;
}

void VertexArray::Bind() const {
    glBindVertexArray(ID);
}

void VertexArray::Unbind() const {
    glBindVertexArray(0);
}

void VertexArray::SetIndexSize(unsigned int size) {
    indexSize = size;
}

unsigned VertexArray::GetIndexSize() {
    return indexSize;
}
