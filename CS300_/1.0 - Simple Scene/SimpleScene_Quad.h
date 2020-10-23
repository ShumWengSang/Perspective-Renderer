//
// Created by pushpak on 6/1/18.
//

#ifndef SIMPLE_SCENE_SIMPLESCENE_QUAD_H
#define SIMPLE_SCENE_SIMPLESCENE_QUAD_H

#include "../Common/Scene.h"
#include "Renderer/Shader.h"

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
    UniquePtr<ObjectManager> objManager;

    // Obj object.
    SharedPtr<Object>       objLoadedObject;
    SharedPtr<Object>       sphereObject[8];
    SharedPtr<Object>       sphereLineObject;
    SharedPtr<Object>       planeObject;

    ShaderLibrary           shaderLibrary;
    SharedPtr<Shader>       diffuseShader;
    SharedPtr<Shader>       generateNormalShader;
    SharedPtr<Shader>       lineShader;
    SharedPtr<Shader>       phongLighting;

    UniquePtr<Camera>       mainCamera;

    // ImGUI changable settings
    // Light
    glm::vec3 lightDirection = glm::vec3(0.7f, 0.7f, 0.7f);;
    glm::vec3 lightColor = glm::vec3(0.5f,0.3f, 0.5f);
    glm::vec4 lineColor = glm::vec4(0.5f,0.7f,0.3f, 1.0f);

    glm::vec3 backgroundColor = glm::vec3(0.3f,0.3f,0.3f);

    // Path sphere scale + pos
    glm::vec3 spherePathPosition = glm::vec3(0, -0.1, 0);
    glm::vec3 spherePathScale = glm::vec3(3);

    // Plane transform
    glm::vec3 planePosition = glm::vec3(0.0f,0.0f,0.0f);
    glm::vec3 planeScale = glm::vec3(2);
    glm::vec3 planeRotation = glm::vec3(0.0f,0.0f,0.0f);

    // ImGUI states
    bool show_demo_window = true;
    bool show_another_window = false;
    int showNormals = 0;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};

void ImGui_ShaderLibrary(ShaderLibrary & shaderlib);

#endif //SIMPLE_SCENE_SIMPLESCENE_QUAD_H
