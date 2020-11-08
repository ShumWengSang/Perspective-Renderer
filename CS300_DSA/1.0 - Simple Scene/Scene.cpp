//
// Created by pushpak on 3/28/18.
//
#include "stdafx.h"
#include "Scene.h"

void APIENTRY OpenGLMessageCallback(unsigned source,
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

Scene::Scene() : _windowWidth(100), _windowHeight(100)
{
}

Scene::Scene(int windowWidth, int windowHeight)
{
    _windowHeight = windowHeight;
    _windowWidth = windowWidth;

}

Scene::~Scene()
{
    CleanUp();
}

// Public methods

// Init: called once when the scene is initialized
int Scene::Init()
{
    // We init and setup OPENGL things here
#ifdef DEBUG_MODE
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback((GLDEBUGPROC)OpenGLMessageCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    return -1;
}

// LoadAllShaders: This is the placeholder for loading the shader files
void Scene::LoadAllShaders()
{
    return;
}


// preRender : called to setup stuff prior to rendering the frame
int Scene::preRender()
{
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);
    glClearDepth(1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, _windowWidth,_windowHeight);
    return -1;
}

// Render : per frame rendering of the scene
int Scene::Render()
{
    return -1;
}

// postRender : Any updates to calculate after current frame
int Scene::postRender()
{
    return -1;
}

// CleanUp : clean up resources before destruction
void Scene::CleanUp()
{
    return;
}

// Display : Per-frame execution of the scene
int Scene::Display()
{
    preRender();

    Render();

    postRender();

    return -1;
}