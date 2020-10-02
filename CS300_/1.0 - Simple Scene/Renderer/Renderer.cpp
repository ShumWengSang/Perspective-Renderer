//
// Created by roland on 9/27/20.
//
#include "../stdafx.h"
#include "Renderer.h"
#include "Shader.h"
#include "Buffer.h"
#include "RenderCommand.h"
#include "Camera.h"

UniquePtr<Renderer::SceneData> Renderer::sceneData = CreateUniquePtr<Renderer::SceneData>();

void Renderer::Init() {
    RenderCommand::Init();
}

void Renderer::Shutdown() {

}

void Renderer::EndScene() {

}

void Renderer::Submit(const SharedPtr<Shader> &shader, const SharedPtr<VertexArray> &vertexArray,
                      const glm::mat4 &transform) {
    shader->Bind();
    shader->SetMat4("model", transform);
    shader->SetMat4("viewProjection", Renderer::sceneData->ViewProjectionMatrix);
    vertexArray->Bind();
    RenderCommand::DrawIndexed(vertexArray);
}

void Renderer::OnWindowsResize(unsigned int width, unsigned int height) {
    RenderCommand::SetViewport(0,0,width, height);
}

void Renderer::BeginScene(Camera const & camera) {
    glm::mat4 perspective =  Camera::perspective(camera);
    glm::mat4 view = Camera::view(camera);
    Renderer::sceneData->ViewProjectionMatrix = perspective * view;
}


