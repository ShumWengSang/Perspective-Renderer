//
// Created by roland on 9/30/20.
//

#ifndef SIMPLE_SCENE_OBJECT_H
#define SIMPLE_SCENE_OBJECT_H


class Object {
public:
    void Update();
    void SetUpdate(std::function<void(Object& obj)> func);
    glm::mat4 GetModelMatrix();
    void SetModelMatrix(glm::mat4 const & model);
    glm::mat4& GetModelMatrixRef();
private:
    std::function<void(Object& obj)> func;
    glm::mat4 ModelMatrix = glm::mat4(1.0f);
};


#endif //SIMPLE_SCENE_OBJECT_H
