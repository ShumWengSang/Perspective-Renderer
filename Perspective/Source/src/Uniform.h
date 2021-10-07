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

    Uniform(Uniform &other) = delete;

    Uniform(Uniform &&other) = delete;

    Uniform &operator=(Uniform &other) = delete;

    Uniform &operator=(Uniform &&other) = delete;

    ~Uniform() = default;


    void UpdateUniformIfNeeded(GLuint program);

    inline void SetUniform(GLuint program, GLint location, T const &value);

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
    this->value = initialValue;
}

template<typename T>
void Uniform<T>::UpdateUniformIfNeeded(GLuint program) {
    if (location == 0 || program != lastProgram) {
        location = glGetUniformLocation(program, name);
    }

    if (value != lastValue || program != lastProgram) {
        SetUniform(program, location, value);
        lastValue = value;
    }

    lastProgram = program;
}

template<typename T>
void Uniform<T>::SetUniform(GLuint program, GLint location, const T &value) {
    if constexpr(std::is_same_v<T, GLint>) glProgramUniform1i(program, location, value);
    else if constexpr(std::is_same_v<T, GLuint>) glProgramUniform1ui(program, location, value);
    else if constexpr(std::is_same_v<T, bool>) glProgramUniform1ui(program, location, value);
    else if constexpr(std::is_same_v<T, GLfloat>) glProgramUniform1f(program, location, value);
    else if constexpr(std::is_same_v<T, GLdouble>) glProgramUniform1d(program, location, value);
    else if constexpr(std::is_same_v<T, glm::vec2>) glProgramUniform2fv(program, location, 1, glm::value_ptr(value));
    else if constexpr(std::is_same_v<T, glm::vec3>) glProgramUniform3fv(program, location, 1, glm::value_ptr(value));
    else if constexpr(std::is_same_v<T, glm::vec4>) glProgramUniform4fv(program, location, 1, glm::value_ptr(value));
    else if constexpr(std::is_same_v<T, glm::ivec2>)glProgramUniform2iv(program, location, 1, glm::value_ptr(value));
    else if constexpr(std::is_same_v<T, glm::ivec3>)glProgramUniform3iv(program, location, 1, glm::value_ptr(value));
    else if constexpr(std::is_same_v<T, glm::ivec4>)glProgramUniform4iv(program, location, 1, glm::value_ptr(value));
    else if constexpr(std::is_same_v<T, glm::uvec2>)glProgramUniform2uiv(program, location, 1, glm::value_ptr(value));
    else if constexpr(std::is_same_v<T, glm::uvec3>)glProgramUniform3uiv(program, location, 1, glm::value_ptr(value));
    else if constexpr(std::is_same_v<T, glm::uvec4>)glProgramUniform4uiv(program, location, 1, glm::value_ptr(value));
    else if constexpr(std::is_same_v<T, glm::quat>) glProgramUniform4fv(program, location, 1, glm::value_ptr(value));
    else if constexpr(std::is_same_v<T, glm::mat3>)
        glProgramUniformMatrix3fv(program, location, 1, GL_FALSE, glm::value_ptr(value));
    else if constexpr(std::is_same_v<T, glm::mat4>)
        glProgramUniformMatrix4fv(program, location, 1, GL_FALSE, glm::value_ptr(value));
    else throw std::runtime_error("unsupported type");

}


#endif //OPENGLFRAMEWORK_UNIFORM_H
