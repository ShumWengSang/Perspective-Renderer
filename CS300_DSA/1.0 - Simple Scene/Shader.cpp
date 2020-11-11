//
// Created by user on 11/4/2020.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: Shader.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Simple_Scene
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 11/4/2020
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "Shader.h"

static std::string getFileName(std::string_view s) {

    char sep = '/';

#ifdef _WIN32
    sep = '\\';
#endif

    size_t i = s.rfind(sep, s.length());
    if (i != std::string::npos) {
        return std::string(s.substr(i+1, s.length() - i));
    }

    return("");
}

std::tuple<GLuint, SharedPtr<VertShaderProg>, SharedPtr<GeomShaderProg>, SharedPtr<FragShaderProg>>
Shader::CreateProgram(
        std::string_view vertFilePath, std::string_view geoFilePath
        , std::string_view fragFilePath
                     ) {
    GLuint vert = -1u, frag = -1u, geo = -1u;
    if(!vertFilePath.empty())
    {
        auto const vertSrc = ReadFile(vertFilePath);
        auto const vPtr = vertSrc.data();
        vert = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, &vPtr);
        if(ValidateProgram(vert, vertFilePath))
        {
            // vert = defaultFallbackShaderVert->GetID();
        }
    }

    if(!geoFilePath.empty())
    {
        auto const geoSrc = ReadFile(geoFilePath);
        auto const gPtr = geoSrc.data();
        geo = glCreateShaderProgramv(GL_GEOMETRY_SHADER, 1, &gPtr);
        ValidateProgram(geo, geoFilePath);
    }

    if(!fragFilePath.empty())
    {
        auto const fragSrc = ReadFile(fragFilePath);
        auto const fPtr = fragSrc.data();
        frag = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &fPtr);
        ValidateProgram(frag, geoFilePath);
    }
    GLuint pipeline = 0;

    glCreateProgramPipelines(1, &pipeline);
    if(vert != -1u )
    {
        glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vert);
    }
    if(geo != -1u)
    {
        glUseProgramStages(pipeline, GL_GEOMETRY_SHADER_BIT, geo);
    }
    if(frag != -1u)
    {
        glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, frag);
    }

    return std::make_tuple(pipeline,
                           vert != -1u ? CreateSharedPtr<ShaderObject>(vert, vertFilePath) : nullptr,
                           geo != -1u ? CreateSharedPtr<ShaderObject>(geo, geoFilePath) : nullptr,
                           frag != -1u ? CreateSharedPtr<ShaderObject>(frag, fragFilePath) : nullptr);
}

std::string Shader::ReadFile(std::string_view fileName) {
    std::cout << "Reading file " << fileName << std::endl;
    std::string result;
    std::ifstream in(std::string(fileName), std::ios::in | std::ios::binary);
    if(in)
    {
        in.seekg(0, std::ios::end);
        size_t size = in.tellg();

        // Resize to size, place it all in.
        if(size != -1u)
        {
            result.resize(size);
            in.seekg(0, std::ios::beg);
            in.read(&result[0], size);
        }
        else
        {
            // ERROR READING FILE
            DEBUG_BREAKPOINT();
        }
    }
    else
    {
        // ERROR OPENING FILE
        DEBUG_BREAKPOINT();
    }
    return result;
}

bool Shader::ValidateProgram(GLuint shaderProgram, std::string_view filename) {
    GLint compiled = 0;
    glProgramParameteri(shaderProgram, GL_PROGRAM_SEPARABLE, GL_TRUE);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &compiled);
    if (compiled == GL_FALSE)
    {
        std::array<char, 1024> compiler_log;
        glGetProgramInfoLog(shaderProgram, compiler_log.size(), nullptr, compiler_log.data());

        std::ostringstream message;
        message << "shader " << filename << " contains error(s):\n\n" << compiler_log.data() << '\n';
        std::clog << message.str();

        // glDeleteShader(shaderProgram);
       // glDeleteProgram(shaderProgram);
        return false;
    }
    return true;
}

SharedPtr<ShaderObject> Shader::CreateProgram(std::string_view shaderPath, GLenum ShaderFlags) {
    auto const shaderSrc = ReadFile(shaderPath);
    auto const vPtr = shaderSrc.data();
    auto const shaderProgram = glCreateShaderProgramv(ShaderFlags, 1, &vPtr);
    if(ValidateProgram(shaderProgram, shaderPath))
    {
        return CreateSharedPtr<ShaderObject>(shaderProgram, shaderPath);
    }
    else
        return nullptr;
}

void Shader::SetFallbackShader(const SharedPtr<ShaderObject> &vert, const SharedPtr<ShaderObject> &frag) {
//    defaultFallbackShaderVert = vert;
//    defaultFallbackShaderFrag = frag;
}

ShaderObject::ShaderObject(GLuint id, std::string_view fileName) : shaderProgramID(id), fileName(fileName),
    shaderName(getFileName(fileName)){

}

ShaderObject::~ShaderObject() {
    DestroyShader();
}

GLuint ShaderObject::GetID() const {
    return shaderProgramID;
}

void ShaderObject::SetID(GLuint id) {
    shaderProgramID = id;
}

void ShaderObject::DestroyShader() {
    glDeleteProgram(shaderProgramID);
}

std::string_view ShaderObject::GetName() const {
    return this->shaderName;
}

std::string_view ShaderObject::GetFile() const {
    return this->fileName;
}
