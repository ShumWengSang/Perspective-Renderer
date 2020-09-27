//
// Created by roland on 9/27/20.
//
#include "../stdafx.h"
#include "RenderCommand.h"

UniquePtr<RendererAPI> RenderCommand::theRendererAPI = RendererAPI::Create();

void RenderCommand::Init() {
    theRendererAPI->Init();
}

void RenderCommand::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
    theRendererAPI->SetViewport(x, y, width, height);
}

void RenderCommand::SetClearColor(const glm::vec4 &color) {
    theRendererAPI->SetClearColor(color);
}

void RenderCommand::Clear() {
    theRendererAPI->Clear();
}

void RenderCommand::DrawIndexec(SharedPtr<VertexArray> const &vertexArray, unsigned int count) {
    theRendererAPI->DrawIndexed(vertexArray, count);
}
