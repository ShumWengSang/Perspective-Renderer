//
// Created by roland on 9/24/20.
//

#ifndef SIMPLE_SCENE_OPENGLRENDERERAPI_H
#define SIMPLE_SCENE_OPENGLRENDERERAPI_H

#include "../Renderer/RendererAPI.h"

class VertexArray;

class OpenGLRendererAPI : public RendererAPI {
public:
    void Init() override;

    void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) override;

    void SetClearColor(const glm::vec4 &color) override;

    void Clear() override;

    void DrawIndexed(SharedPtr<VertexArray> const &vertexArray, unsigned int indexCount) override;
};


#endif //SIMPLE_SCENE_OPENGLRENDERERAPI_H
