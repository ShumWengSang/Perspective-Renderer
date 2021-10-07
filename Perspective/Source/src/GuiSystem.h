//
// Created by user on 1/25/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: GuiSystem.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/25/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_GUISYSTEM_H
#define OPENGLFRAMEWORK_GUISYSTEM_H

#pragma region Internal

// Taken from ImGui Example
struct ExampleAppConsole {
    char InputBuf[256];
    ImVector<char *> Items;
    ImVector<const char *> Commands;
    ImVector<char *> History;
    int HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter Filter;
    bool AutoScroll;
    bool ScrollToBottom;

    ExampleAppConsole();

    ~ExampleAppConsole();

    // Portable helpers
    static int Stricmp(const char *s1, const char *s2);

    static int Strnicmp(const char *s1, const char *s2, int n);

    static char *Strdup(const char *s);

    static void Strtrim(char *s);

    void ClearLog();

    void AddLog(const char *fmt, ...);

    void AddError(const char *fmt, ...);

    void Draw(const char *title, bool *p_open);

    void ExecCommand(const char *command_line);

    // In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
    static int TextEditCallbackStub(ImGuiInputTextCallbackData *data);

    int TextEditCallback(ImGuiInputTextCallbackData *data);
};

#pragma endregion

class Input;

class GuiSystem {
public:
    static GuiSystem &getInstance() {
        static GuiSystem instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    GuiSystem(GuiSystem const &) = delete;

    void operator=(GuiSystem const &) = delete;

private:
    GuiSystem();

public:
    void Init(GLFWwindow *window);

    void Destroy();

    void NewFrame();

    void RenderDrawData(ImDrawData *data);

    // Utilities
    void Texture(GLuint texture, float aspectRatio = 16.0f / 9.0f);

    // Add to ImGui Gui
    void AddGui(std::string const &name, std::function<void(void)> const &GuiFunction);

    static bool IsUsingMouse();

    static bool IsUsingKeyboard();

    ExampleAppConsole console;
private:
    GLFWwindow *window;

    GLuint *shaderProgram;

    GLuint indexBuffer;
    GLuint fontTexture;

    GLFWcursor *cursors[ImGuiMouseCursor_COUNT] = {0};

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Hold a bunch of ImGui functions. This refreshes every frame, so possible optimization is to
    // templatize it so it holds lambas
    std::vector<std::pair<std::string, std::function<void(void)>>> GuiFunctions;

    bool showConsole = true;
    bool mainDockspace = true;
public:
    void DockspaceBegin();

    void DockspaceEnd();

};


#endif //OPENGLFRAMEWORK_GUISYSTEM_H
