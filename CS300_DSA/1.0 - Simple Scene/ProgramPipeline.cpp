//
// Created by user on 11/5/2020.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: ProgramPipeline.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Simple_Scene
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 11/5/2020
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "ProgramPipeline.h"
#include "Shader.h"

ProgramPipeline::ProgramPipeline(GLuint id, std::string_view name)
: ShaderObjects((size_t)ShaderType::MAX) , pipelineID(id), Name(name)
{

}

ProgramPipeline::~ProgramPipeline() {
    Destroy();
}

std::tuple<bool, bool, bool> ProgramPipeline::doShaderExist() const {
    bool one = false, two = false, three = false;
    if(ShaderObjects[0] != nullptr)
        one = true;
    if(ShaderObjects[1] != nullptr)
        two = true;
    if(ShaderObjects[2] != nullptr)
        three = true;

    return std::make_tuple(one, two, three);
}

std::tuple<SharedPtr<ShaderObject> const &, SharedPtr<ShaderObject> const &, SharedPtr<ShaderObject> const &>
ProgramPipeline::GetShaders() const {
    return std::make_tuple(ShaderObjects[0], ShaderObjects[1], ShaderObjects[2]);
}

void ProgramPipeline::ReloadShader(std::initializer_list<ShaderType> shaders) {
    for (size_t i = 0; i < shaders.size(); i++)
    {
        ReloadShader(ShaderObjects[(unsigned)(*(shaders.begin() + i))], (*(shaders.begin() + i)));
    }
}

void ProgramPipeline::ReloadShader(ProgramPipeline::ShaderType shader) {
    ReloadShader(ShaderObjects[(unsigned)(shader)], shader);
}

void ProgramPipeline::ReloadShaders() {
    ReloadShader(ShaderObjects[(unsigned)(ShaderType::Vertex)], ShaderType::Vertex);
    ReloadShader(ShaderObjects[(unsigned)(ShaderType::Geometry)], ShaderType::Geometry);
    ReloadShader(ShaderObjects[(unsigned)(ShaderType::Fragment)], ShaderType::Fragment);
}

void ProgramPipeline::PushShaders(std::initializer_list<SharedPtr<ShaderObject>> const & list) {
    if(list.size() >= (unsigned)ShaderType::MAX)
        DEBUG_BREAKPOINT();
    ShaderObjects.insert(ShaderObjects.begin(), list);
}

void ProgramPipeline::Destroy() {
    glDeleteProgramPipelines(1, &pipelineID);
}

SharedPtr<ShaderObject> ProgramPipeline::ReloadShader(const SharedPtr<ShaderObject> &shr, ShaderType type) {

    GLenum glType;
    GLenum programPipelineBit;
    switch(type)
    {
        case ShaderType::Vertex:
            glType = GL_VERTEX_SHADER;
            programPipelineBit = GL_VERTEX_SHADER_BIT;
            break;
        case ShaderType::Fragment:
            glType = GL_FRAGMENT_SHADER;
            programPipelineBit = GL_FRAGMENT_SHADER_BIT;
            break;
        case ShaderType::Geometry:
            glType = GL_GEOMETRY_SHADER;
            programPipelineBit = GL_GEOMETRY_SHADER_BIT;
            break;
        default:
            DEBUG_BREAKPOINT();
            return nullptr;
    }

    auto reloadedShader = Shader::CreateProgram(shr->GetFile(), glType);

    // If loading successful
    if(reloadedShader) {
        // Replace the shader in the pipeline with the newly created one
        glUseProgramStages(pipelineID, programPipelineBit, reloadedShader->GetID());
        // Now we replace the one inside our buffers, and destroy the old one.
        ShaderObjects[(unsigned) type] = reloadedShader;
    }
    else
    {
        std::cout << shr->GetFile() << " has errors!" << std::endl;
    }
    return reloadedShader;
}

void ProgramPipeline::PushShader(const SharedPtr<ShaderObject> &shaderObject, ProgramPipeline::ShaderType type) {
    ShaderObjects[(unsigned)type] = shaderObject;
}

void ProgramPipeline::Bind() {
    glBindProgramPipeline(pipelineID);
}

void ProgramPipeline::Unbind() {
    glBindProgramPipeline(0);
}

SharedPtr<ShaderObject> const &ProgramPipeline::GetShader(ProgramPipeline::ShaderType type) const {
    return ShaderObjects[(unsigned)type];
}
