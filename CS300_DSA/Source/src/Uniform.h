//
// Created by user on 1/27/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: Uniform.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/27/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_UNIFORM_H
#define OPENGLFRAMEWORK_UNIFORM_H

template<typename T>
class Uniform {
public:
    Uniform(const char *name, T initialValue);
    Uniform(Uniform& other) = delete;
    Uniform(Uniform&& other) = delete;
    Uniform& operator=(Uniform& other) = delete;
    Uniform& operator=(Uniform&& other) = delete;
    ~Uniform() = default;


    void UpdateUniformIfNeeded(GLuint program);



    GLint location = 0;
    T value = std::numeric_limits<T>::max();

private:
    const char *name;
    GLuint lastProgram = 0;
    T lastValue = T{};
};

template<typename T>
Uniform<T>::Uniform(const char *name, T initialValue) {
    this->name = name;

    assert(value != initialValue);
    this->value = initialValue;
}

template<typename T>
void Uniform<T>::UpdateUniformIfNeeded(GLuint program) {
    if (location == 0 || program != lastProgram)
    {
        location = glGetUniformLocation(program, name);
    }

    if (value != lastValue || program != lastProgram)
    {
        prospect::internal::PerformUniformUpdate(program, location, value);
        lastValue = value;
    }

    lastProgram = program;
}


#endif //OPENGLFRAMEWORK_UNIFORM_H
