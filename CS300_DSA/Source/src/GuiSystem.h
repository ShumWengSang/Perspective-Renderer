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

class Input;

class GuiSystem {
public:
    static GuiSystem& getInstance()
    {
        static GuiSystem instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    GuiSystem(GuiSystem const&) = delete;
    void operator=(GuiSystem const&)  = delete;
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
    void AddGui(std::string const & name, std::function<void(void)> const & GuiFunction);

    static bool IsUsingMouse() ;
    static bool IsUsingKeyboard() ;
private:
    GLFWwindow *window;

    GLuint *shaderProgram;

    GLuint indexBuffer;
    GLuint fontTexture;

    GLFWcursor *cursors[ImGuiMouseCursor_COUNT] = { 0 };

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Hold a bunch of ImGui functions. This refreshes every frame, so possible optimization is to
    // templatize it so it holds lambas
    std::vector<std::pair<std::string, std::function<void(void)>>> GuiFunctions;

};


#endif //OPENGLFRAMEWORK_GUISYSTEM_H
