#include "stdafx.h"
#include "Logger.h"
#include "Input.h"
#include "App.h"
#include "TransformSystem.h"
#include "TextureSystem.h"
#include "ModelSystem.h"
#include "ShaderSystem.h"
#include "GuiSystem.h"
#include "GlobalVariables.h"

static std::unique_ptr<App> app;
static bool renderUI = true;

void glfwResizeCallback(GLFWwindow *window, int width, int height)
{
    app->windowWidth = width;
    app->windowHeight = height;
    app->Resize(width, height);
}

int main(int argc, char *argv[])
{
    // Set global from argv
    if(argc >= 2)
    {
        Global::loadFile = argv[1];
    }

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

    using namespace glm;
    std::vector<vec3> points = {
            {0.2113249,0.5608486,0.3076091},
            {0.7560439,0.6623569,0.9329616},
            {0.0002211,0.7263507,0.2146008},
            {0.3303271,0.1985144,0.312642 },
            {0.6653811,0.5442573,0.3616361},
            {0.6283918,0.2320748,0.2922267},
            {0.8497452,0.2312237,0.5664249},
            {0.685731 ,0.2164633,0.4826472},
            {0.8782165,0.8833888,0.3321719},
            {0.068374 ,0.6525135,0.5935095}};
    printf("MIDTERM =========== \n");


    vec3 mininum = vec3(0.0002211, 0.1985144, 0.2146008);
    vec3 maximum = vec3(0.8782165, 0.8833888,0.9329616);

    vec3 midPoint = (mininum + maximum) / 2.0f;
    std::cout << glm::to_string(midPoint) << std::endl;

    float maxDistance = -9999999;
    glm::vec3 maxPointAway;
    for(int i = 0; i < points.size(); i++)
    {
        float distance = glm::distance(midPoint, points[i]);
        if(std::abs(distance) > maxDistance)
        {
            maxDistance = std::abs(distance);
            maxPointAway = points[i];
        }
    }

    printf("Max point away is %s, with a distance of %f\n", glm::to_string(maxPointAway).c_str(), maxDistance);

    std::vector<vec3> Dictionary = {{1,1,0}, {1,-1,0}, {1,0,1},{1,0,-1},{0,1,1},
                         {0,1,-1},{0,-1,2}};
    std::vector<std::pair<vec3, vec3>> ExternalPoints;
    for(int j = 0; j < Dictionary.size(); ++j)
    {
        vec3 min = {999,999,999};
        vec3 max = {-999,-999,-999};
        float dotMin = std::numeric_limits<float>::max();
        float dotMax = std::numeric_limits<float>::min();

        for(int i = 0; i < points.size(); ++i)
        {
            float dot = glm::dot(points[i], Dictionary[j]);

            if(dot > dotMax)
            {
                max = points[i];
                dotMax = dot;
            }
            if(dot < dotMin)
            {
                min = points[i];
                dotMin = dot;
            }
        }

        printf("Min Max of normal %s are %s, %s \n",
               glm::to_string(Dictionary[j]).c_str(), glm::to_string(min).c_str(), glm::to_string(max).c_str());
        ExternalPoints.emplace_back(std::make_pair(min, max));
    }
    // Find extreme point. Find central point of sphere points
    std::pair<vec3,vec3> Extreme;
    float oldRadius;
    vec3 oldCenter = vec3(0);
    {
        float maxDistance = std::numeric_limits<float>::min();
        for (int i = 0; i < ExternalPoints.size(); ++i) {
            float distance = std::abs(glm::distance(ExternalPoints[i].first, ExternalPoints[i].second));
            if(distance > maxDistance)
            {
                maxDistance = distance;
                Extreme = ExternalPoints[i];
            }
            oldCenter += ExternalPoints[i].first;
            oldCenter += ExternalPoints[i].second;
        }
        oldCenter = (Extreme.first + Extreme.second) / 2.0f;
        oldRadius = maxDistance / 2;
    }

    {
        // See if we need to resize going through every point
        for(int i = 0; i < points.size(); ++i)
        {
            float d = glm::distance(oldCenter, points[i]);
            if(std::abs(d) > oldRadius)
            {
                float oldR = oldRadius;
                // We need to resize to fit the new point
                oldRadius = (oldRadius + std::abs(d)) / 2.0f;
                oldCenter = oldCenter + (oldRadius - oldR) * glm::normalize(points[i] - oldCenter);
            }
        }
        // Final results
        printf("Circle - Center of %s with radius of %f\n", glm::to_string(oldCenter).c_str(), oldRadius);
    }

    for(int i = 0; i < points.size(); i++)
    {
        vec3 newPoint = points[i] * vec3(1.5,2.0,2.0);
        printf("%s\n", glm::to_string(newPoint).c_str());
    }

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
        guiSystem.DockspaceBegin();
        app->Draw(input, deltaTime, accumulatedTime);
        accumulatedTime += deltaTime;
        guiSystem.DockspaceEnd();
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