//
// Created by pushpak on 6/1/18.
//

#ifndef SIMPLE_SCENE_SIMPLESCENE_QUAD_H
#define SIMPLE_SCENE_SIMPLESCENE_QUAD_H

#include "../Common/Scene.h"

class Mesh;
class VertexArray;
class Shader;
class Camera;
class Object;
class ObjectManager;

class SimpleScene_Quad : public Scene
{

public:
    SimpleScene_Quad() = default;
    SimpleScene_Quad( int windowWidth, int windowHeight );
    virtual ~SimpleScene_Quad();


public:
    int Init() override;
    void CleanUp() override;

    int Render() override;
    int postRender() override;
    int preRender() override;

private:

    // member functions
    void initMembers();

    // This is the non-software engineered portion of the code
    // Go ahead and modularize the VAO and VBO setup into
    // BufferManagers and ShaderManagers
    void SetupBuffers();

    void SetupNanoGUI(GLFWwindow *pWwindow) override;

    std::vector<GLfloat>    geometryBuffer;
    GLfloat                 angleOfRotation;

private:
    // Obj object.
    SharedPtr<Object>       objLoadedObject;
    SharedPtr<Object>       faceNormalLine;
    SharedPtr<Object>       vertexNormalLine;

    UniquePtr<ObjectManager> objManager;

    SharedPtr<Object>       sphereObject[8];
    SharedPtr<Object>       sphereFaceNormalLine[8];
    SharedPtr<Object>       sphereVertexNormalLine[8];

    SharedPtr<Shader>       phongShader;
    SharedPtr<Shader>       lineShader;
    UniquePtr<Camera>       mainCamera;

    // ImGUI changable settings
    // Light
    glm::vec3 lightDirection = glm::vec3(0.7f, 0.7f, 0.7f);;
    glm::vec3 lightColor = glm::vec3(0.0f,0.0f, -10.0f);
    glm::vec4 lineColor = glm::vec4(0.5f,0.7f,0.3f, 1.0f);

    // ImGUI states
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};


#endif //SIMPLE_SCENE_SIMPLESCENE_QUAD_H
