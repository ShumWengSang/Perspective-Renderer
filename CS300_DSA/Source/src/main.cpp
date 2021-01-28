#include "stdafx.h"
#include "Logger.h"
#include "Input.h"
#include "App.h"
#include "TransformSystem.h"
#include "TextureSystem.h"
#include "ModelSystem.h"
#include "ShaderSystem.h"
#include "GuiSystem.h"

static std::unique_ptr<App> app;
static bool renderUI = true;

void glfwResizeCallback(GLFWwindow *window, int width, int height)
{
    app->windowWidth = width;
    app->windowHeight = height;
    app->Resize(width, height);
}

int main()
{
    // Setup basic GLFW and context settings
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit())
    {
        LogError("Fatal error: could not initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create app (should require no GL context!)
    app = AppSelector::ConstructApp();
    App::Settings settings = app->Setup();

    glfwWindowHint(GLFW_RESIZABLE, settings.window.resizeable);
    glfwWindowHint(GLFW_SAMPLES, settings.context.msaaSamples);

    // Create window
    GLFWwindow *window = nullptr;
    if (settings.window.fullscreen)
    {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *vidmode = glfwGetVideoMode(monitor);
        window = glfwCreateWindow(vidmode->width, vidmode->height, settings.window.title.c_str(), monitor, nullptr);
    }
    else
    {
        int width = settings.window.size.width;
        int height = settings.window.size.height;
        window = glfwCreateWindow(width, height, settings.window.title.c_str(), nullptr, nullptr);
    }
    if (!window)
    {
        LogError("Fatal error: could not created the requested window");
    }

    // Setup OpenGL context
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Setup OpenGL error handlers
    glad_set_post_callback(GladPostCallback);

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(GLDebugMessageCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);

    // Setup input and callbacks
    Input input;
    glfwSetWindowUserPointer(window, &input);

    glfwSetKeyCallback(window, Input::KeyEventCallback);
    glfwSetMouseButtonCallback(window, Input::MouseButtonEventCallback);
    glfwSetCursorPosCallback(window, Input::MouseMovementEventCallback);
    glfwSetScrollCallback(window, Input::MouseScrollEventCallback);

    // glfwSetCharCallback(window, GuiSystem::CharacterInputCallback);

    // Enable important and basic features
    //glEnable(GL_FRAMEBUFFER_SRGB); (note: implemented manually!)
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // Initialize global systems (that need initialization)
    TransformSystem& transformSystem = TransformSystem::getInstance();
    TextureSystem& textureSystem = TextureSystem::getInstance();
    ModelSystem& modelSystem = ModelSystem::getInstance();
    ShaderSystem& shaderSystem = ShaderSystem::getInstance();
    GuiSystem& guiSystem = GuiSystem::getInstance();

    transformSystem.Init();
    textureSystem.Init();
    modelSystem.Init();
    guiSystem.Init(window);

    app->Init();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glfwSetFramebufferSizeCallback(window, glfwResizeCallback);

    // (manually initiate first callback)
    glfwResizeCallback(nullptr, width, height);
    glfwSwapInterval(settings.window.vsync ? 1 : 0);

    // Main loop

    glfwSetTime(0.0);
    double lastTime = glfwGetTime();
    float accumulatedTime = 0.0;

    while (!glfwWindowShouldClose(window))
    {
        input.PreEventPoll();
        glfwPollEvents();

        transformSystem.Update();
        textureSystem.Update();
        modelSystem.Update();
        shaderSystem.Update();

        double currentTime = glfwGetTime();
        double elapsedTime = currentTime - lastTime;
        lastTime = currentTime;
        float deltaTime = float(elapsedTime);


        guiSystem.NewFrame();

        app->Draw(input, deltaTime, accumulatedTime);
        accumulatedTime += deltaTime;

        if (renderUI)
        {
            ImGui::Render();
            guiSystem.RenderDrawData(ImGui::GetDrawData());
        }
        else
        {
            ImGui::EndFrame();
        }

        glfwSwapBuffers(window);
    }
    guiSystem.Destroy();
    modelSystem.Destroy();
    textureSystem.Destroy();
    transformSystem.Destroy();


    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}