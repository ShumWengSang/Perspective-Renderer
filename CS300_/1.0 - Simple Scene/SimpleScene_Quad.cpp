//
// Created by pushpak on 6/1/18.
//

#include "stdafx.h"
#include "SimpleScene_Quad.h"


static char ModelPath[] = "../../Common/models/";

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void SimpleScene_Quad::SetupNanoGUI(GLFWwindow *pWindow)
{
    pWindow = nullptr;
}


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
SimpleScene_Quad::~SimpleScene_Quad()
{
    initMembers();
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
SimpleScene_Quad::SimpleScene_Quad(int windowWidth, int windowHeight) : Scene(windowWidth, windowHeight),
//                                                                        programID(0), vertexbuffer(0), VertexArrayID(0),
                                                                        angleOfRotation(0.0f)
{
    initMembers();
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-unused-return-value"
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void SimpleScene_Quad::initMembers()
{
//    programID = 0;
//    vertexbuffer = 0;
//    VertexArrayID = 0;

    geometryBuffer.clear();
    angleOfRotation = 0.0f;
}
#pragma clang diagnostic pop

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void SimpleScene_Quad::CleanUp()
{
    // Cleanup VBO
//    glDeleteBuffers(1, &vertexbuffer);
//    glDeleteVertexArrays(1, &VertexArrayID);
//    glDeleteProgram(programID);
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void SimpleScene_Quad::SetupBuffers()
{
    // We are remaking the example scene to prove our architecture works.
//    quadVertexArray = VertexArray::Create();
//
//    SharedPtr<VertexBuffer> quadVB = CreateSharedPtr<VertexBuffer>(geometryBuffer, sizeof(geometryBuffer));
//    quadVB->SetLayout({
//                              { ShaderDataType::Float, "vPosition"}
//    });
//    quadVertexArray->AddVertexBuffer(quadVB);

//    glGenVertexArrays(1, &VertexArrayID);
//    glBindVertexArray(VertexArrayID);
//
//    glGenBuffers(1, &vertexbuffer);
//
//    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//    glBufferData(GL_ARRAY_BUFFER, geometryBuffer.size() * sizeof(GLfloat),
//                 geometryBuffer.data(), GL_STATIC_DRAW);


    meshVertexArray = VertexArray::Create();
    // Add attribute
    SharedPtr<VertexBuffer> meshVB= VertexBuffer::Create(
            (float*)geometryBuffer.data(),
            sizeof(GLfloat) * geometryBuffer.size());
    meshVB->SetLayout({
                              { ShaderDataType::Float4, "vPosition"}
    });
    meshVertexArray->AddVertexBuffer(meshVB);

    unsigned int Indices[] = { 0, 3, 1,
                               1, 3, 2,
                               2, 3, 0,
                               0, 1, 2 };

    // Add index array
    SharedPtr<IndexBuffer> meshIB = IndexBuffer::Create(
            Indices,
            sizeof(Indices) / sizeof(Indices[0]));
    meshVertexArray->SetIndexBuffer(meshIB);
    return;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SimpleScene_Quad::Init()
{
    // Create and compile our GLSL program from the shaders
//    exampleShader.SetID(
//            LoadShaders("../../Common/shaders/QuadVertexShader.vert",
//                        "../../Common/shaders/QuadFragmentShader.frag")
//    );
    defaultShader = Shader::Create("DefaultQuad",
                                   "../../Common/shaders/QuadVertexShader.vert",
                                   "../../Common/shaders/QuadFragmentShader.frag");


    // Just two triangles making up a quad
//    geometryBuffer = { 0.0f, 0.0f, 0.0f,
//                        1.0f, 0.0f, 0.0f,
//                        0.0f, 1.0f, 0.0f,
//                        1.0f, 0.0f, 0.0f,
//                        1.0f, 1.0f, 0.0f,
//                        0.0f, 1.0f, 0.0f
//    };
    geometryBuffer = { 0.0f, 0.0f, 0.0f,
                        1.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f,
                        1.0f, 1.0f, 0.0f,
    };
//    geometryBuffer = { 1.0f, -1.0f, 0.0f,
//                       0.0f, -1.0f, 1.0f,
//                       1.0f, -1.0f, 0.0f,
//                       0.0f, 1.0f, 0.0f};

    // Load the obj file
    std::string obj = "cube.obj";
    std::string FullPath = ModelPath + obj;

    auto mesh = ObjLoader::LoadObj(FullPath.c_str());
    // Check if mesh was loaded
    if(mesh)
    {
        this->mesh = std::move(mesh.value());
    }
    else
    {
        std::cout << FullPath << " mesh not loaded. Not showing mesh" << std::endl;
    }

    SetupBuffers();

    return Scene::Init();
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SimpleScene_Quad::Render()
{
    RenderCommand::SetClearColor({0.5f,0.5f,0.5f,0.5f});
    RenderCommand::Clear();

    meshVertexArray->Bind();
    meshVertexArray->GetVertexBuffers()[0]->Bind();
    //glBindBuffer(GL_ARRAY_BUFFER, );
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer( 0,
//                            4,
//                            GL_FLOAT,
//                            GL_FALSE,
//                            0,
//                           (void *) 0 );

    // Uniform matrix
    // Uniform transformation (vertex shader)
    // GLint vTransformLoc = glGetUniformLocation(programID, "vertexTransform");

    // Draw the triangle !
    // T * R * S * Vertex
    glm::mat4 modelMat = glm::mat4(1.0f);
    glm::vec3 scaleVector = glm::vec3(100000.0f);
    glm::vec3 centroid = glm::vec3( -0.5f, -0.5f, 0.0f );
    modelMat = glm::rotate(angleOfRotation, glm::vec3(0.0f, 0.0f, 1.0f)) *
            glm::scale( scaleVector ) *
            glm::translate( centroid );

//    glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &modelMat[0][0]);

//    defaultShader->Bind();
//    defaultShader->SetMat4("vertexTransform", modelMat);

    Renderer::Submit(defaultShader, meshVertexArray, modelMat);
    //glDrawArrays(GL_TRIANGLES, 0, 6);

    //glDisableVertexAttribArray(0);

    return 0;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SimpleScene_Quad::postRender()
{
    angleOfRotation += 0.01f;
    return 0;
}
