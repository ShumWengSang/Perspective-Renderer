//
// Created by roland on 9/30/20.
//
#include "stdafx.h"
#include "Object.h"

#include <utility>

void Object::Update() {
    if(func != nullptr)
        func(*this);
}

void Object::SetUpdate(std::function<void(Object &)> func) {
    this->func = std::move(func);
}

glm::mat4 Object::GetModelMatrix() {
    return ModelMatrix;
}

void Object::SetModelMatrix(const glm::mat4 &model) {
    ModelMatrix = model;
}

