//
// Created by user on 11/4/2020.
//

//
// Created by user on 11/4/2020.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: Shader.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Simple_Scene
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 11/4/2020
 * End Header --------------------------------------------------------*/


#ifndef SIMPLE_SCENE_SHADER_H
#define SIMPLE_SCENE_SHADER_H

#include "Shader.h"

class ShaderObject {
public:
    ShaderObject(GLuint id, std::string_view fileName);
    ~ShaderObject();
    GLuint GetID() const;
    void SetID(GLuint id);
    void DestroyShader();
    std::string_view GetName() const;
    std::string_view GetFile() const;

    ShaderObject(ShaderObject const &) = delete;
    ShaderObject& operator=(const ShaderObject&) = delete;

    template <typename T>
    inline void SetUniform(GLint location, T const & value)
    {
        if		constexpr(std::is_same_v<T, GLint>)		glProgramUniform1i(shaderProgramID, location, value);
        else if constexpr(std::is_same_v<T, GLuint>)	glProgramUniform1ui(shaderProgramID, location, value);
        else if constexpr(std::is_same_v<T, bool>)		glProgramUniform1ui(shaderProgramID, location, value);
        else if constexpr(std::is_same_v<T, GLfloat>)	glProgramUniform1f(shaderProgramID, location, value);
        else if constexpr(std::is_same_v<T, GLdouble>)	glProgramUniform1d(shaderProgramID, location, value);
        else if constexpr(std::is_same_v<T, glm::vec2>) glProgramUniform2fv(shaderProgramID, location, 1, glm::value_ptr(value));
        else if constexpr(std::is_same_v<T, glm::vec3>) glProgramUniform3fv(shaderProgramID, location, 1, glm::value_ptr(value));
        else if constexpr(std::is_same_v<T, glm::vec4>) glProgramUniform4fv(shaderProgramID, location, 1, glm::value_ptr(value));
        else if constexpr(std::is_same_v<T, glm::ivec2>)glProgramUniform2iv(shaderProgramID, location, 1, glm::value_ptr(value));
        else if constexpr(std::is_same_v<T, glm::ivec3>)glProgramUniform3iv(shaderProgramID, location, 1, glm::value_ptr(value));
        else if constexpr(std::is_same_v<T, glm::ivec4>)glProgramUniform4iv(shaderProgramID, location, 1, glm::value_ptr(value));
        else if constexpr(std::is_same_v<T, glm::uvec2>)glProgramUniform2uiv(shaderProgramID, location, 1, glm::value_ptr(value));
        else if constexpr(std::is_same_v<T, glm::uvec3>)glProgramUniform3uiv(shaderProgramID, location, 1, glm::value_ptr(value));
        else if constexpr(std::is_same_v<T, glm::uvec4>)glProgramUniform4uiv(shaderProgramID, location, 1, glm::value_ptr(value));
        else if constexpr(std::is_same_v<T, glm::quat>) glProgramUniform4fv(shaderProgramID, location, 1, glm::value_ptr(value));
        else if constexpr(std::is_same_v<T, glm::mat3>) glProgramUniformMatrix3fv(shaderProgramID, location, 1, GL_FALSE, glm::value_ptr(value));
        else if constexpr(std::is_same_v<T, glm::mat4>) glProgramUniformMatrix4fv(shaderProgramID, location, 1, GL_FALSE, glm::value_ptr(value));
        else throw std::runtime_error("unsupported type");
    }

private:
    GLuint shaderProgramID;
    std::string fileName;
    std::string shaderName;
};

using VertShaderProg = ShaderObject;
using GeomShaderProg = ShaderObject;
using FragShaderProg = ShaderObject;

class Shader {
public:
    static std::string ReadFile(std::string_view fileName);

    static std::tuple<GLuint, SharedPtr<VertShaderProg>, SharedPtr<GeomShaderProg>, SharedPtr<FragShaderProg>>
    CreateProgram(
            std::string_view vertFilePath,
            std::string_view geoFilePath,
            std::string_view fragFilePath);

    static void SetFallbackShader(SharedPtr<ShaderObject> const & vert, SharedPtr<ShaderObject> const & frag);
    static SharedPtr<ShaderObject> CreateProgram(std::string_view shaderPath, GLenum  ShaderFlags);
    protected:
    friend class ProgramPipeline;

    static bool ValidateProgram(GLuint shaderProgram, std::string_view filename);

//    static SharedPtr<ShaderObject> defaultFallbackShaderVert;
//    static SharedPtr<ShaderObject> defaultFallbackShaderFrag;
};


#endif //SIMPLE_SCENE_SHADER_H
