//
// Created by roland on 9/30/20.
//

#ifndef SIMPLE_SCENE_OBJECT_H
#define SIMPLE_SCENE_OBJECT_H


class VertexArray;
class Shader;

class Object {
private:
    glm::mat4 transform = {};
    SharedPtr<VertexArray> VAO;
public:
    // Transform
    const glm::mat4 & GetTransformMatrix() const;
    void SetTransform(glm::mat4 const& transform);

    // VAO
    void SetVAO(SharedPtr<VertexArray> const & VAO, SharedPtr<Shader> const & shader);
    SharedPtr<VertexArray> & GetVAO();

    // Shader
    std::vector<SharedPtr<Shader>> const & GetShaders() const;
    void AddShaderPass(SharedPtr<Shader> const & shader);
    void ReplaceShader(SharedPtr<Shader> const & shader);

    // Children
    std::vector<SharedPtr<Object>> children;
    void AddChild(SharedPtr<Object> const & child);

    // Update function
    void SetUpdate(std::function<void(Object&, glm::mat4 const &)> lambda);
    void Update(glm::mat4 const & objMatrix = glm::mat4(1.0f));

    bool Renderable = true;
private:
    std::function<void(Object&, glm::mat4 const &)> updateFunction;
    std::vector<SharedPtr<Shader>> shaders;
};


#endif //SIMPLE_SCENE_OBJECT_H
