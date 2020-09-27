//
// Created by roland on 9/24/20.
//
#include "../stdafx.h"
#include "OpenGLRendererAPI.h"

void OpenGLMessageCallback(unsigned source,
                           unsigned type,
                           unsigned id,
                           unsigned severity,
                           int length,
                           const char* message,
                           const void* userPAram)
{
    switch(severity)
    {
        case GL_DEBUG_SEVERITY_HIGH: printf("HIGH SEVERITY: %s", message);
        case GL_DEBUG_SEVERITY_MEDIUM: printf("MEDIUM SEVERITY: %s", message);
        case GL_DEBUG_SEVERITY_LOW: printf("LOW SEVERITY: %s", message);
        case GL_DEBUG_SEVERITY_NOTIFICATION: printf("Trace: %s", message);
        default: printf("OpenGL Message callback %s", message);
    }
}

void OpenGLRendererAPI::Init() {
#ifdef DEBUG_MODE
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(OpenGLMessageCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, GL_FALSE);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRendererAPI::DrawIndexed(SharedPtr<VertexArray> const &vertexArray, unsigned int indexCount) {
    unsigned count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

}
