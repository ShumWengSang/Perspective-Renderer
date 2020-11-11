//
// Created by user on 11/5/2020.
//

//
// Created by user on 11/5/2020.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: ProgramPipeline.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Simple_Scene
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 11/5/2020
 * End Header --------------------------------------------------------*/


#ifndef SIMPLE_SCENE_PROGRAMPIPELINE_H
#define SIMPLE_SCENE_PROGRAMPIPELINE_H
class ShaderObject;
class ProgramPipeline {
public:
    enum class ShaderType : unsigned
    {
        Vertex = 0,
        Geometry = 1,
        Fragment = 2,
        MAX = 3
    };
    ProgramPipeline(GLuint id, std::string_view name);
    ~ProgramPipeline();

    void PushShaders(std::initializer_list<SharedPtr<ShaderObject>> const & list);
    void PushShader(SharedPtr<ShaderObject> const & shaderObject, ShaderType type);

    std::tuple<bool, bool, bool> doShaderExist() const;

    std::tuple<SharedPtr<ShaderObject> const &, SharedPtr<ShaderObject> const &, SharedPtr<ShaderObject>  const &> GetShaders() const;
    SharedPtr<ShaderObject> const & GetShader(ShaderType type) const;

    void ReloadShader(std::initializer_list<ShaderType> shaders);
    void ReloadShader(ShaderType shader);
    void ReloadShaders();

    void Bind();
    void Unbind();

    ProgramPipeline(ProgramPipeline const &) = delete;
    ProgramPipeline& operator=(const ProgramPipeline&) = delete;
public:
    SharedPtr<ShaderObject> ReloadShader(SharedPtr<ShaderObject> const & shr, ShaderType type);
    void Destroy();

    GLuint pipelineID;
    std::vector<SharedPtr<ShaderObject>> ShaderObjects;
    std::string Name;
};


#endif //SIMPLE_SCENE_PROGRAMPIPELINE_H
