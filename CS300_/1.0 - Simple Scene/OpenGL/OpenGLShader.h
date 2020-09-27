//
// Created by roland on 9/24/20.
//
#ifndef SIMPLE_SCENE_OPENGLSHADER_H
#define SIMPLE_SCENE_OPENGLSHADER_H
#include "../Renderer/Shader.h"

class OpenGLShader : public Shader
{
public:
    explicit OpenGLShader(std::string const & filepath);
    OpenGLShader(std::string const & name, std::string const & vertexSrc, std::string const & fragmentSrc);
    ~OpenGLShader() override;

    void Bind() const override;

    void Unbind() const override;

    void SetInt(const std::string &name, int value) override;

    void SetFloat(const std::string &name, float value) override;

    void SetFloat3(const std::string &name, const glm::vec3 &value) override;

    void SetFloat4(const std::string &name, const glm::vec4 &value) override;

    void SetMat4(const std::string &name, const glm::mat4 &value) override;

    void SetMat3(const std::string &name, const glm::mat3 &value) override;

    const std::string &GetName() const override;

    //////////////////////////////////////////////////////
private:
    unsigned RendererID;
    std::string Name;

private:
    static std::string ReadFile(std::string const & fileName);
    static GLuint Compile(std::string const & shaderContent, GLenum shaderType);
    void LinkProgram(GLuint shaderID);

};


#endif //SIMPLE_SCENE_OPENGLSHADER_H
