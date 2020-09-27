//
// Created by roland on 9/24/20.
//

#ifndef SIMPLE_SCENE_RENDERERAPI_H
#define SIMPLE_SCENE_RENDERERAPI_H


class RendererAPI {
public:
    enum class API{
        None = 0,
        OpenGL = 1
    };

    virtual ~RendererAPI() = default;
    virtual void Init() = 0;
    virtual void SetViewport( unsigned x, unsigned y, unsigned width, unsigned height) = 0;
    virtual void SetClearColor(glm::vec4 const & color) = 0;
    virtual void Clear() = 0;

    virtual void DrawIndexed(SharedPtr<VertexArray> const & vertexArray, unsigned indexCount) = 0;
    static UniquePtr<RendererAPI> Create();
    static API GetAPI() {return api;}

private:
    static API api;
};


#endif //SIMPLE_SCENE_RENDERERAPI_H
