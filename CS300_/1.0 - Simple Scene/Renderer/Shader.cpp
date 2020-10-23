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

SharedPtr<Shader> Shader::Create(const string &name, const string &vertexSrc, const string &fragmentSrc,
                                 const string &geomSrc) {
    return CreateSharedPtr<OpenGLShader>(name, vertexSrc, fragmentSrc, geomSrc);
}


bool ShaderLibrary::Exists(const string &name) const {
    return Shaders.find(name) != Shaders.end();
}

SharedPtr<Shader> ShaderLibrary::Get(const string &name) {
    if(!this->Exists(name))
    {
        // Shader does not exists?
        DEBUG_BREAKPOINT();
        return DefaultShader;
    }
    return Shaders[name];
}

SharedPtr<Shader> ShaderLibrary::Load(const string &name, const string &filepath) {

    try {
        auto shader = Shader::Create(filepath);
        Add(name, shader);
        return shader;
    }
    catch (std::exception& exception) {
        return DefaultShader;
    }

}

SharedPtr<Shader> ShaderLibrary::Load(const string &filepath) {
    try {
        auto shader = Shader::Create(filepath);
        Add(shader);
        return shader;
    }
    catch (std::exception& exception) {
        return DefaultShader;
    }
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

    // If debug, we set the shader source here.
    ShaderFilePaths[name] = std::make_tuple(
            shader->GetPath(0),shader->GetPath(1), shader->GetPath(2));
}

const SharedPtr<Shader>& ShaderLibrary::Reload(const string &name) {
    if(!this->Exists(name))
    {
        // Shader already exists?
        DEBUG_BREAKPOINT();
    }
    auto& tuple = ShaderFilePaths[name];
    // Load the shader from the stored filenames
    std::string const & vertexPath = std::get<0>(tuple);
    std::string const & fragPath = std::get<1>(tuple);
    std::string const & geoPath = std::get<2>(tuple);

    Shaders[name]->Reload(vertexPath, fragPath, geoPath);
    return Shaders[name];
}

ShaderLibrary::ShaderLibrary() : DefaultShader(Shader::Create("Default Shader",
                                                              "../Shaders/DefaultShader.vert",
                                                              "../Shaders/DefaultShader.frag")){

}
