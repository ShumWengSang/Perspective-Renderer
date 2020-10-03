//
// Created by roland on 10/1/20.
//
#include "stdafx.h"
#include "ObjectManager.h"
#include "Object.h"
#include "Renderer/Renderer.h"


void ObjectManager::RenderAllObject() {
    for(unsigned i = 0; i < Objects.size(); ++i)
    {
        for(auto& shader : Objects[i]->GetShaders())
        {
            if(Objects[i]->Renderable) {
                Renderer::Submit(
                        shader,
              Objects[i]->GetVAO(),
               Objects[i]->GetTransformMatrix());
            }
        }
    }
}

SharedPtr<Object> ObjectManager::CreateObject() {
    SharedPtr<Object> obj = CreateSharedPtr<Object>();
    this->Objects.emplace_back(obj);
    return obj;
}
