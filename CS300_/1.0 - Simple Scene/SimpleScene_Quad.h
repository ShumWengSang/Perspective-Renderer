//
// Created by pushpak on 6/1/18.
//

#ifndef SIMPLE_SCENE_SIMPLESCENE_QUAD_H
#define SIMPLE_SCENE_SIMPLESCENE_QUAD_H

#include "../Common/Scene.h"
#include "ObjLoader.h"
#include "VertexDataManager.h"
#include "shader.hpp"

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


private:

    // member functions
    void initMembers();

    // This is the non-software engineered portion of the code
    // Go ahead and modularize the VAO and VBO setup into
    // BufferManagers and ShaderManagers
    void SetupBuffers();

    void SetupNanoGUI(GLFWwindow *pWwindow) override;

    // data members
//    GLuint  programID;
//    GLuint  vertexbuffer;
//    GLuint  VertexArrayID;
    GLuint vertexBuffer;

    std::vector<GLfloat>    geometryBuffer;
    GLfloat   angleOfRotation;

    MeshUniquePtr mesh;
    VertexDataManager exampleVertexDataManager;
    Shader exampleShader;
};


#endif //SIMPLE_SCENE_SIMPLESCENE_QUAD_H
