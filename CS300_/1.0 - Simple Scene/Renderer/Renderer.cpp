//
// Created by roland on 9/27/20.
//
#include "../stdafx.h"
#include "Renderer.h"

UniquePtr<Renderer::SceneData> Renderer::sceneData = CreateUniquePtr<Renderer::SceneData>();

void Renderer::Init() {

}

void Renderer::Shutdown() {

}

void Renderer::BeginScene() {

}

void Renderer::EndScene() {

}

void Renderer::Submit(const SharedPtr<Shader> &shader, const SharedPtr<VertexArray> &vertexArray,
                      const glm::mat4 &transform) {
    shader->Bind();
    shader->SetMat4("u_ViewProjection", sceneData->ViewProjectionMatrix);
    shader->SetMat4("u_Transform", transform);

    vertexArray->Bind();
    RenderCommand::DrawIndexed(vertexArray);
}

