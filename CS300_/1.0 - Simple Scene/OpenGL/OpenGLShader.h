//
// Created by roland on 9/24/20.
//
#ifndef SIMPLE_SCENE_OPENGLSHADER_H
#define SIMPLE_SCENE_OPENGLSHADER_H
#include "../Renderer/Shader.h"

enum ShaderType : unsigned
{
    VertexShader = 0,
    FragmentShader,
    GeometryShader,
    MaxShader
};

class OpenGLShader : public Shader
{
public:
    explicit OpenGLShader(std::string const & filepath);
    OpenGLShader(std::string const & name, std::string const & vertexSrc, std::string const & fragmentSrc,
                 std::string const & geometrySrc = std::string());
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

    const std::string& GetPath(int pathID) const override;
    //////////////////////////////////////////////////////
    void Reload(const std::string &vertexSrc, const std::string &fragmentSrc, const std::string &geomSrc) override;

    void SetUniformBuffer(UniformBufferObject &uniformBuffer) override;
    void AddShaderReloadedCallback(const ShaderReloadedCallback& callback) override;
private:
    unsigned RendererID;
    std::string Name;
    std::string ShaderPath[MaxShader];
    std::vector<ShaderReloadedCallback> CallbackList;
private:
    static std::string ReadFile(std::string const & fileName);
    static GLuint Compile(std::string const & shaderContent, GLenum shaderType);
    void LinkPrograms(GLuint *shaderIDs, unsigned size, unsigned id);
private:
    static unsigned currentBind;
    friend class ShaderLibrary;

    unsigned int
    InitializeShader(const std::string &vertexSrc, const std::string &fragmentSrc, const std::string &geometricSrc);
};


#endif //SIMPLE_SCENE_OPENGLSHADER_H
