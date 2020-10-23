//
// Created by roland on 9/24/20.
//
#include "../stdafx.h"
#include "OpenGLShader.h"

unsigned OpenGLShader::currentBind = 0;

void OpenGLShader::Bind() const {
    if(currentBind != RendererID) {
        glUseProgram(RendererID);
        currentBind = RendererID;
    }
}

void OpenGLShader::Unbind() const {
    glUseProgram(0);
    currentBind = 0;
}

void OpenGLShader::SetInt(const std::string &name, int value) {
    GLint location = glGetUniformLocation(RendererID, name.c_str());
    if(location == -1) DEBUG_BREAKPOINT();
    glUniform1i(location, value);
}

void OpenGLShader::SetFloat(const std::string &name, float value) {
    GLint location = glGetUniformLocation(RendererID, name.c_str());
    if(location == -1) DEBUG_BREAKPOINT();
    glUniform1f(location, value);
}

void OpenGLShader::SetFloat3(const std::string &name, const glm::vec3 &value) {
    GLint location = glGetUniformLocation(RendererID, name.c_str());
    if(location == -1) DEBUG_BREAKPOINT();
    glUniform3f(location, value.x, value.y, value.z);
}

void OpenGLShader::SetFloat4(const std::string &name, const glm::vec4 &value) {
    GLint location = glGetUniformLocation(RendererID, name.c_str());
    if(location == -1) DEBUG_BREAKPOINT();
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void OpenGLShader::SetMat3(const std::string &name, const glm::mat3 &value) {
    GLint location = glGetUniformLocation(RendererID, name.c_str());
    if(location == -1) DEBUG_BREAKPOINT();
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void OpenGLShader::SetMat4(const std::string &name, const glm::mat4 &value) {
    GLint location = glGetUniformLocation(RendererID, name.c_str());
    if(location == -1) DEBUG_BREAKPOINT();
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

const std::string &OpenGLShader::GetName() const {
    return Name;
}

std::string OpenGLShader::ReadFile(const std::string &fileName) {

    std::cout << "Reading file " << fileName << std::endl;
    std::string result;
    std::ifstream in(fileName, std::ios::in | std::ios::binary);
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

GLuint OpenGLShader::Compile(const std::string &shaderContent, GLenum shaderType) {
    // Create the shaders
    GLuint shaderID = glCreateShader(shaderType);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Shader
#ifdef DEBUG_OUT
    std::string shader;
    switch(shaderType)
    {
        case GL_VERTEX_SHADER: shader = "GL_VERTEX_SHADER"; break;
        case GL_FRAGMENT_SHADER: shader = "GL_FRAGMENT_SHADER"; break;
        case GL_GEOMETRY_SHADER: shader = "GL_GEOMETRY_SHADER"; break;
        default: DEBUG_BREAKPOINT();
    }
    printf("Compiling shader %s: \n", shader.data());
#endif
    const char * content = shaderContent.data();
    glShaderSource(shaderID, 1, &content , nullptr);
    glCompileShader(shaderID);

    // Check Shader
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(shaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
#ifdef DEBUG_OUT
        printf("%s\n", &VertexShaderErrorMessage[0]);
#endif
        throw std::exception();
    }
    return shaderID;
}

// This is main constructor
OpenGLShader::OpenGLShader(const std::string &name, const std::string &vertexSrc, const std::string &fragmentSrc,
                           const std::string &geometricSrc) : Name(name), ShaderPath{vertexSrc, fragmentSrc, geometricSrc}{
    RendererID = InitializeShader(vertexSrc, fragmentSrc, geometricSrc);


}

unsigned OpenGLShader::InitializeShader(const std::string &vertexSrc, const std::string &fragmentSrc,
                                        const std::string &geometricSrc) {// Read the vertex program
    int i = 0;
    GLuint shaderIDs[3] = {};
    std::string shaderContent = ReadFile(vertexSrc);
    shaderIDs[i++] = Compile(shaderContent, GL_VERTEX_SHADER);

    if(!geometricSrc.empty())
    {
        shaderContent = ReadFile(geometricSrc);
        shaderIDs[i++] = Compile(shaderContent, GL_GEOMETRY_SHADER);
    }

    shaderContent = ReadFile(fragmentSrc);
    shaderIDs[i++] = Compile(shaderContent, GL_FRAGMENT_SHADER);


    unsigned newID = glCreateProgram(); // Create ProgramID BEFORE linking
    LinkPrograms(shaderIDs, i, newID);
    return newID;
}

#ifdef _WIN32
char sep = '\\'
#elif linux
char sep = '/';
#endif
OpenGLShader::OpenGLShader(const std::string &filepath) :  OpenGLShader(filepath.substr(
                                                                            filepath.rfind(sep, filepath.length()) + 1,
                                                                            filepath.length() -
                                                                            filepath.rfind(sep, filepath.length())),
                                                                        filepath + ".vertex",
                                                                        filepath + ".fragment",std::string())
                                                           {
    // Divert to main constructor.

}

OpenGLShader::~OpenGLShader() {
    glDeleteProgram(RendererID);
}

void OpenGLShader::LinkPrograms(GLuint *shaderIDs, unsigned int size, unsigned id) {
    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Link the program
#ifdef DEBUG_OUT
    printf("Linking program\n");
#endif

    for(unsigned i = 0; i < size; ++i)
    {
        glAttachShader(id, shaderIDs[i]);
    }
    glLinkProgram(id);

    // Check the program
    glGetProgramiv(id, GL_LINK_STATUS, &Result);
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(id, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
        DEBUG_BREAKPOINT();
        throw std::exception();
    }
    for(unsigned i = 0; i < size; ++i)
    {
        glDetachShader(id, shaderIDs[i]);
        glDeleteShader(shaderIDs[i]);
    }
}

const std::string &OpenGLShader::GetPath(int pathID) const {
    if(pathID < 0 || pathID >= MaxShader)
        DEBUG_BREAKPOINT();
    return ShaderPath[pathID];
}

SharedPtr<Shader>
OpenGLShader::Reload(const std::string &vertexSrc, const std::string &fragmentSrc, const std::string &geomSrc) {
    RendererID = InitializeShader(vertexSrc, fragmentSrc, geomSrc);
}
