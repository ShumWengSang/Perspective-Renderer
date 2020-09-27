//
// Created by roland on 9/24/20.
//
#include "../stdafx.h"
#include "RendererAPI.h"
#include "../OpenGL/OpenGLRendererAPI.h"
UniquePtr<RendererAPI> RendererAPI::Create() {
    return CreateUniquePtr<OpenGLRendererAPI>();
}
