//
// Created by user on 1/24/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: Input.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/24/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "Input.h"

void Input::PreEventPoll() {
    // Reset keyboard pressed & released keys for this frame
    memset(wasKeyPressed, false, KEYBOARD_KEY_COUNT * sizeof(bool));
    memset(wasKeyReleased, false, KEYBOARD_KEY_COUNT * sizeof(bool));

    // Reset & update mouse
    memset(wasButtonPressed, false, MOUSE_BUTTON_COUNT * sizeof(bool));
    memset(wasButtonReleased, false, MOUSE_BUTTON_COUNT * sizeof(bool));
    lastXPosition = currentXPosition;
    lastYPosition = currentYPosition;
    lastScrollOffset = currentScollOffset;
}

bool Input::IsKeyDown(int key) const {
    return isKeyDown[key];
}

bool Input::WasKeyPressed(int key) const {
    return wasKeyPressed[key];
}

bool Input::WasKeyReleased(int key) const {
    return wasKeyPressed[key];
}

void Input::KeyEventCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    auto input = static_cast<Input *>(glfwGetWindowUserPointer(window));

    switch (action) {
        case GLFW_PRESS:
            input->wasKeyPressed[key] = true;
            input->isKeyDown[key] = true;
            break;

        case GLFW_RELEASE:
            input->wasKeyReleased[key] = true;
            input->isKeyDown[key] = false;
            break;

        case GLFW_REPEAT:
            // Irrelevant
            break;

        default:
            break;
    }
}

bool Input::IsButtonDown(int button) const {
    return isButtonDown[button];
}

bool Input::WasButtonPressed(int button) const {
    return wasButtonPressed[button];
}

bool Input::WasButtonReleased(int button) const {
    return wasButtonReleased[button];
}

glm::vec2 Input::GetMousePosition(GLFWwindow *window) const {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    return glm::vec2(static_cast<float>(x), static_cast<float>(y));
}

glm::vec2 Input::GetMouseDelta() const {
    return glm::vec2(currentXPosition - lastXPosition, currentYPosition - lastYPosition);
}

float Input::GetScrollDelta() const {
    return static_cast<float>(currentScollOffset - lastScrollOffset);
}

void Input::MouseButtonEventCallback(GLFWwindow *window, int button, int action, int mods) {
    auto input = static_cast<Input *>(glfwGetWindowUserPointer(window));

    switch (action) {
        case GLFW_PRESS:
            input->wasButtonPressed[button] = true;
            input->isButtonDown[button] = true;
            break;

        case GLFW_RELEASE:
            input->wasButtonReleased[button] = true;
            input->isButtonDown[button] = false;
            break;

        case GLFW_REPEAT:
            // TODO: Handle repeat events!
            break;

        default:
            break;
    }
}

void Input::MouseMovementEventCallback(GLFWwindow *window, double xPos, double yPos) {
    auto input = static_cast<Input *>(glfwGetWindowUserPointer(window));

    input->currentXPosition = xPos;
    input->currentYPosition = yPos;

    if (input->lastXPosition == -1.0f) {
        input->lastXPosition = xPos;
        input->lastYPosition = yPos;
    }
}

void Input::MouseScrollEventCallback(GLFWwindow *window, double xOffset, double yOffset) {
    auto input = static_cast<Input *>(glfwGetWindowUserPointer(window));

    // Ignore x-offset
    input->currentScollOffset += yOffset;
}
