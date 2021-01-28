//
// Created by user on 1/24/2021.
//


/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: App.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/24/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_APP_H
#define OPENGLFRAMEWORK_APP_H
class Input;
class App;
namespace AppSelector
{
    std::unique_ptr<App> ConstructApp();
}

class App {
public:
    struct Settings
    {
        Settings() = default;

        struct
        {
            std::string title{ "Prospect Renderer" };

            struct { int width; int height; } size;
            bool fullscreen = false;

            bool resizeable = false;
            bool vsync = true;
        } window;

        struct
        {
            int msaaSamples = 0;
        } context;
    };

public:
    App() = default;
    virtual ~App() = default;

    int windowWidth = 0;
    int windowHeight = 0;

    ///////////////////////////////////////////////////////////////////////////
    // Application lifetime

    // Called before any window or context is created, so must not call any GL code
    virtual Settings Setup() = 0;

    // Called on app creation. Window etc. does exist by this point
    virtual void Init() = 0;

    // Called on default framebuffer resize, and right after Init
    virtual void Resize(int width, int height) = 0;

    ///////////////////////////////////////////////////////////////////////////
    // Drawing / main loop

    // Called every new frame
    virtual void Draw(Input const & input, float deltaTime, float runningTime) = 0;

    // We can only have on App in the game right?
    static App& GetApp();

private:
    inline static App* CurrApp;
    friend std::unique_ptr<App> AppSelector::ConstructApp();
};

#endif //OPENGLFRAMEWORK_APP_H
