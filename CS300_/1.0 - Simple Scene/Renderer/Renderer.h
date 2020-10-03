//
// Created by roland on 9/27/20.
//

#ifndef SIMPLE_SCENE_RENDERER_H
#define SIMPLE_SCENE_RENDERER_H

class Shader;
class VertexArray;
class Camera;

class Renderer {
public:
    static void Init();
    static void Shutdown();

    static void BeginScene(Camera const & camera);
    static void EndScene();

    static void OnWindowsResize(unsigned width, unsigned height);

    static void Submit(SharedPtr<Shader> const & shader, SharedPtr<VertexArray> const & vertexArray,
                       glm::mat4 const & transform = glm::mat4(1.0f));

private:
    struct SceneData
    {
        glm::mat4 ViewMatrix;
        glm::mat4 ProjectionMatrix;
    };
    static UniquePtr<SceneData> sceneData;
};


#endif //SIMPLE_SCENE_RENDERER_H
