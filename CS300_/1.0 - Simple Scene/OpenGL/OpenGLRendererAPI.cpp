//
// Created by roland on 9/24/20.
//
#include "../stdafx.h"
#include "OpenGLRendererAPI.h"
#include "../Renderer/Buffer.h"

void OpenGLMessageCallback(unsigned source,
                           unsigned type,
                           unsigned id,
                           unsigned severity,
                           int length,
                           const char* message,
                           const void* userPAram)
{
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << " ";

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch(severity)
    {
        case GL_DEBUG_SEVERITY_HIGH: printf("  HIGH SEVERITY: %s\n", message); break;
        case GL_DEBUG_SEVERITY_MEDIUM: printf("  MEDIUM SEVERITY: %s\n", message);break;
        case GL_DEBUG_SEVERITY_LOW: printf("  LOW SEVERITY: %s\n", message); break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: printf("  Trace: %s\n", message); break;
        default: printf("  OpenGL Message callback %s\n", message);
    }
    DEBUG_BREAKPOINT();
}

void OpenGLRendererAPI::Init() {
#ifdef DEBUG_MODE
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(OpenGLMessageCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

}

void OpenGLRendererAPI::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height) {

    glViewport(x,y,width,height);
}

void OpenGLRendererAPI::SetClearColor(const glm::vec4 &color) {
    glClearColor(color.r, color.g, color.b, color.a);
}

void OpenGLRendererAPI::Clear() {
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRendererAPI::DrawIndexed(SharedPtr<VertexArray> const &vertexArray, unsigned int indexCount) {
    unsigned count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();

    GLenum drawType;

    // Determine what drawtype is used.
    switch(vertexArray->GetType())
    {
        case VertexArray::PrimitiveType::Triangles:
            drawType = GL_TRIANGLES;
            break;
        case VertexArray::PrimitiveType::Lines:
            drawType = GL_LINES;
            break;
        case VertexArray::PrimitiveType::None:
            std::cout << "You have to specify draw mode!" << std::endl;
            DEBUG_BREAKPOINT();
            break;
    }
    glDrawElements(drawType, count, GL_UNSIGNED_INT, nullptr);

}
