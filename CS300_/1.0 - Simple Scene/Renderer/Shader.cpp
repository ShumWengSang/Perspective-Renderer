//
// Created by roland on 9/24/20.
//
#include "../stdafx.h"
#include "Shader.h"
#include "../OpenGL/OpenGLShader.h"

using namespace std;


SharedPtr<Shader> Shader::Create(const string &filePath) {
    return CreateSharedPtr<OpenGLShader>(filePath);
}

SharedPtr<Shader> Shader::Create(const string &name, const string &vertexSrc, const string &fragmentSrc) {
    return CreateSharedPtr<OpenGLShader>(name, vertexSrc, fragmentSrc);
}

bool ShaderLibrary::Exists(const string &name) const {
    return Shaders.find(name) != Shaders.end();
}

SharedPtr<Shader> ShaderLibrary::Get(const string &name) {
    if(!this->Exists(name))
    {
        // Shader does not exists?
        DEBUG_BREAKPOINT();
    }
    return Shaders[name];
}

SharedPtr<Shader> ShaderLibrary::Load(const string &name, const string &filepath) {
    auto shader = Shader::Create(filepath);
    Add(name, shader);
    return shader;
}

SharedPtr<Shader> ShaderLibrary::Load(const string &filepath) {
    auto shader = Shader::Create(filepath);
    Add(shader);
    return shader;
}

void ShaderLibrary::Add(const SharedPtr<Shader> &shader) {
    Add(shader->GetName(), shader);
}

void ShaderLibrary::Add(const string &name, const SharedPtr<Shader> &shader) {
    if(this->Exists(name))
    {
        // Shader already exists?
        DEBUG_BREAKPOINT();
    }
    Shaders[name] = shader;
}
