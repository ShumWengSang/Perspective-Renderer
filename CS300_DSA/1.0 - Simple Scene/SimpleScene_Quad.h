//
// Created by pushpak on 6/1/18.
//

#ifndef SIMPLE_SCENE_SIMPLESCENE_QUAD_H
#define SIMPLE_SCENE_SIMPLESCENE_QUAD_H

#include "Scene.h"

const int MAX_LIGHT = 16;

class Mesh;
class Camera;
class VertexArray;
class ProgramPipeline;
class Object;
class MyUBO;
class LightUBO;
class Texture;
template <typename T>
class UniformBuffer;

class SimpleScene_Quad : public Scene
{

public:
    SimpleScene_Quad() = default;
    SimpleScene_Quad( int windowWidth, int windowHeight );
    virtual ~SimpleScene_Quad();
    void SetupNanoGUI(GLFWwindow*) override;

public:
    int Init() override;
    void CleanUp() override;

    int Render() override;
    int postRender() override;
    int preRender() override;

private:

    static void DrawObject(SharedPtr<VertexArray> const & vao, SharedPtr<ProgramPipeline> const & pipeline, SharedPtr<Object> const & obj);
    // member functions
    void initMembers();

    void SetupBuffers();

    std::vector<GLfloat>    geometryBuffer;
    GLfloat                 angleOfRotation{};
    // Meshes
    UniquePtr<Mesh> quadMesh;
    UniquePtr<Mesh> mesh;

    // Objects
    SharedPtr<Object> meshObj;
    SharedPtr<Object> planeObj;
    SharedPtr<Object> sphereObj[MAX_LIGHT];

    // VAO
    SharedPtr<VertexArray> meshArray;
    SharedPtr<VertexArray> planeArray;
    SharedPtr<VertexArray> sphereArray;

    // VBO attribute buffers (used for regenerating)
    GLuint VBOplaneMesh;
    GLuint VBOmesh;

    // Program Pipelines
    SharedPtr<ProgramPipeline> defaultPipeline;
    SharedPtr<ProgramPipeline> PhongDiffuse;
    SharedPtr<ProgramPipeline> NormalLineGeoPipeline;
    SharedPtr<ProgramPipeline> PhongLighting;
    SharedPtr<ProgramPipeline> PhongShading;
    SharedPtr<ProgramPipeline> BlinnShading;
    // UBO
    SharedPtr<UniformBuffer<MyUBO>> uniformBuffer;
    SharedPtr<UniformBuffer<LightUBO>> lightUBO;

    // Textures
    SharedPtr<Texture> DiffuseTexture;
    SharedPtr<Texture> SpecularTexture;

private:
    float cubeAngle = 0.0f;

    // ImGUI changable settings
    // Light
    glm::vec3 lightDirection = glm::vec3(0.7f, 0.7f, 0.7f);;
    glm::vec4 lineColor = glm::vec4(0.5f,0.7f,0.3f, 1.0f);

    glm::vec3 meshPosition = glm::vec3(0);

    // Path sphere scale + pos
    glm::vec3 spherePathPosition = glm::vec3(0, -0.1, 0);
    glm::vec3 spherePathScale = glm::vec3(3);

    // Plane transform
    glm::vec3 planePosition = glm::vec3(0.0f,-2.0f,0.0f);
    glm::vec3 planeScale = glm::vec3(5);
    float rotateAngle = 3.14f / 2;

    // ImGUI states
    bool show_demo_window = true;
    int useShader = 0; // 0 for phong lighting, 1 for phong shader, 2 for blinn
    int showNormals = 0;
    glm::vec3 AmbientColor = glm::vec3(0.1f,0.1f,0.1f);
    glm::vec3 EmissiveColor = glm::vec3(0.0f, 0, 0);
    bool SphereUpdate = true;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    UniquePtr<Camera> mainCamera;

    // UBO Binding
    static constexpr auto UBOBinding = 0;
    static constexpr auto LightUBOBinding = 1;

    // Uniforms
    static constexpr auto uniformModel = 0;
    static constexpr auto uniformFaceMode = 0;
    static constexpr auto uniformLineColor = 0;
    static constexpr auto uniformAmbientColor = 1;
    static constexpr auto uniformEmissiveColor = 2;

    // Phong Diffuse
    static constexpr auto uniformlightColor = 0;
    static constexpr auto uniformLightDirection = 1;

    // Texture binding points
    static constexpr auto DiffuseBinding = 0;
    static constexpr auto SpecularBinding = 1;

    void EditTransform(const Camera& camera, glm::mat4& matrix);
    friend void ImGui_ShaderLibrary(SimpleScene_Quad& scene);
};

// void ImGui_ShaderLibrary(ShaderLibrary & shaderlib);

#endif //SIMPLE_SCENE_SIMPLESCENE_QUAD_H
