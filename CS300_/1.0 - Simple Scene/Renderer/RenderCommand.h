//
// Created by roland on 9/27/20.
//

#ifndef SIMPLE_SCENE_RENDERCOMMAND_H
#define SIMPLE_SCENE_RENDERCOMMAND_H


class RenderCommand {
public:
    static void Init();
    static void SetViewport(unsigned x, unsigned y, unsigned width, unsigned height);
    static void SetClearColor(glm::vec4 const & color);
    static void Clear();
    static void DrawIndexec(SharedPtr<VertexArray> const & vertexArray, unsigned count = 0);
private:
    static UniquePtr<RendererAPI> theRendererAPI;
};


#endif //SIMPLE_SCENE_RENDERCOMMAND_H
