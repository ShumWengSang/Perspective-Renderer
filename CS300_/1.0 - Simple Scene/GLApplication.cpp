// Include standard headers
#include "stdafx.h"


// Local / project headers
#include "../Common/Scene.h"

#include "SimpleScene_Quad.h"

// Function declarations
bool savePPMImageFile(std::string &filepath, std::vector<GLfloat> &pixels, int width, int height);

//////////////////////////////////////////////////////////////////////

GLFWwindow *window;
Scene  *scene;
std::string GLOBAL_OBJFILE = std::string();

int windowWidth = 1024;
int windowHeight = 768;

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    if(argc > 1)
    {
        GLOBAL_OBJFILE = std::string(argv[1]);
    }

    // Setting up OpenGL properties
    glfwWindowHint(GLFW_SAMPLES, 1); // change for anti-aliasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(windowWidth, windowHeight, // window dimensions
                              "Sample 1 - Simple scene (Quad) with Scene Class", // window title
                              nullptr, // which monitor (if full-screen mode)
                              nullptr); // if sharing context with another window
    if (window == nullptr)
    {
        fprintf(stderr,
                "Failed to open GLFW window. If you have an Intel GPU, they are not 4.0 compatible.\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // OpenGL resource model - "glfwCreateWindow" creates the necessary data storage for the OpenGL
    // context but does NOT make the created context "current". We MUST make it current with the following
    // call
    glfwMakeContextCurrent(window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char* glsl_version = "#version 130";
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Initialize GLEW
    glewExperimental = static_cast<GLboolean>(true); // Needed for core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Initialize the scene
    scene = new SimpleScene_Quad( windowWidth, windowHeight );

    // Scene::Init encapsulates setting up the geometry and the texture
    // information for the scene
    scene->Init();

    do
    {
        // Now render the scene
        // Scene::Display method encapsulates pre-, render, and post- rendering operations
        scene->Display();

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

bool savePPMImageFile(std::string &filepath, std::vector<GLfloat> &pixels, int width, int height)
{
    std::ofstream  texFile(filepath);

    texFile << "P3" << std::endl;
    texFile << width << "  " << height << std::endl;
    texFile << "255" << std::endl;

    auto it = pixels.begin();

    for( int row = 0; row < height; ++row )
    {
        for (int col = 0; col < width; ++col)
        {
            texFile << *it++ << " ";
            texFile << *it++ << " ";
            texFile << *it++ << " ";
        }

        texFile << std::endl;
    }

    texFile.close();

    return true;
}
