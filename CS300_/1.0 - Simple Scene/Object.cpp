//
// Created by roland on 9/30/20.
//
#include "stdafx.h"
#include "Object.h"
#include "Renderer/Buffer.h"


const glm::mat4 &Object::GetTransformMatrix() const {
    return this->transform;
}

void Object::SetTransform(const glm::mat4 &transform) {
    this->transform = transform;
}

void Object::AddChild(const SharedPtr<Object> &child) {
    this->children.emplace_back(child);
}

void Object::SetUpdate(std::function<void(Object &, glm::mat4 const &)> lambda) {
        updateFunction = lambda;
}

void Object::Update(const glm::mat4 &objMatrix) {
    if(updateFunction)
        updateFunction(*this, objMatrix);

    for(unsigned i = 0; i < this->children.size(); ++i)
    {
        children[i]->Update(this->GetTransformMatrix());
    }
}

void Object::SetVAO(const SharedPtr<VertexArray> &VAO, const SharedPtr<Shader> &shader) {
    this->shader = shader;
    this->VAO = VAO;
}

SharedPtr<VertexArray> &Object::GetVAO() {
    return this->VAO;
}

SharedPtr<Shader> const &Object::GetShader() const {
    return this->shader;
}
