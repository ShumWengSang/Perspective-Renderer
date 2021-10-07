//
// Created by user on 1/24/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: Input.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/24/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_INPUT_H
#define OPENGLFRAMEWORK_INPUT_H


class Input {
public:
    Input() = default;

    ~Input() = default;

    Input(Input &other) = delete;

    Input &operator=(Input &other) = delete;

    void PreEventPoll();

    // Keyboard stuff
    static const int KEYBOARD_KEY_COUNT{GLFW_KEY_LAST};

    bool IsKeyDown(int key) const;

    bool WasKeyPressed(int key) const;

    bool WasKeyReleased(int key) const;

    static void KeyEventCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    // Mouse related
    static const int MOUSE_BUTTON_COUNT{GLFW_MOUSE_BUTTON_LAST};

    bool IsButtonDown(int button) const;

    bool WasButtonPressed(int button) const;

    bool WasButtonReleased(int button) const;

    glm::vec2 GetMousePosition(GLFWwindow *window) const;

    glm::vec2 GetMouseDelta() const;

    float GetScrollDelta() const;

    static void MouseButtonEventCallback(GLFWwindow *window, int button, int action, int mods);

    static void MouseMovementEventCallback(GLFWwindow *window, double xPos, double yPos);

    static void MouseScrollEventCallback(GLFWwindow *window, double xOffset, double yOffset);

private:
    // Keyboard
    bool isKeyDown[KEYBOARD_KEY_COUNT] = {0};
    bool wasKeyPressed[KEYBOARD_KEY_COUNT] = {0};
    bool wasKeyReleased[KEYBOARD_KEY_COUNT] = {0};

    // Mouse
    bool isButtonDown[MOUSE_BUTTON_COUNT] = {0};
    bool wasButtonPressed[MOUSE_BUTTON_COUNT] = {0};
    bool wasButtonReleased[MOUSE_BUTTON_COUNT] = {0};

    // Should only be used for getting the mouse delta etc. and NOT for querying current position, since this accumulates errors and might "lag" behind.
    double currentXPosition = 0;
    double currentYPosition = 0;
    double lastXPosition = -1;
    double lastYPosition = -1;

    double currentScollOffset = 0;
    double lastScrollOffset = 0;
};


#endif //OPENGLFRAMEWORK_INPUT_H
