//
// Created by user on 1/25/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: ShaderSystem.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/25/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_SHADERSYSTEM_H
#define OPENGLFRAMEWORK_SHADERSYSTEM_H

class ShaderDependant;

// Defines how many programs that the shader loader can manage. Should be set at a safe maximum.
#ifndef MAX_NUM_MANAGED_PROGRAMS
#define MAX_NUM_MANAGED_PROGRAMS 128
#endif

#pragma region Internal Data

struct Shader {
    GLenum type;
    std::string filename;

    Shader(GLenum type, const std::string &filename) : type(type), filename(filename) {}
};

struct Program {
    size_t fixedLocation;
    std::vector<Shader> shaders{};

    bool operator==(const Program &other) const {
        return fixedLocation == other.fixedLocation;
    }
};

template<>
struct std::hash<Program> {
    std::size_t operator()(const Program &program) const {
        return program.fixedLocation;
    }
};

struct GlslFile {
    std::string filename;
    std::time_t timestamp = 0;
    std::unordered_set<Program> dependablePrograms{};

    GlslFile() {}

    explicit GlslFile(const std::string &filename) : filename(filename) {}
};

#pragma endregion

struct ShaderErrorReport {
    std::string shaderName;
    std::string errorMessage;;
    std::string preprocessedSource;
};


class ShaderSystem {
public:
    static ShaderSystem &getInstance() {
        static ShaderSystem instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    ShaderSystem(ShaderSystem const &) = delete;

    void operator=(ShaderSystem const &) = delete;

private:
    ShaderSystem() = default;

public:
    // Reload recompile and relink shaders/programs if needed. Call this every
    // render loop iteration before using any shaders managed by this shader loader.
    void Update();

    std::vector<ShaderErrorReport> GetShaderErrorReports();

    //

    // Add a shader program with the specified file name (*.vert.glsl and *.frag.glsl assumed)
    // Supply a shader depandant if there is some object that should be updated with this program
    GLuint *AddProgram(const std::string &name, ShaderDependant *shaderDependant = nullptr);

    // Add a shader program with the specified names for the vertex and fragment shaders
    // Supply a shader depandant if there is some object that should be updated with this program
    GLuint *
    AddProgram(const std::string &vertName, const std::string &fragName, ShaderDependant *shaderDependant = nullptr);

    void AddProgram(
            GLuint **programOut, const std::string &vertName, const std::string &fragName
            , ShaderDependant *shaderDependant = nullptr
                   );

    void AddComputeProgram(GLuint **programOut, const std::string &name, ShaderDependant *shaderDependant = nullptr);

    GLuint *AddComputeProgram(const std::string &name, ShaderDependant *shaderDependant = nullptr);

private:
    void AddManagedFile(const std::string &filename, const Program &dependableProgram);

    std::time_t GetTimestamp(const GlslFile &file) const;

    bool FileReadable(const std::string &filename) const;

    void ReadFileWithIncludes(
            const std::string &filename, const Program &dependableProgram, std::stringstream &sourceBuffer
                             );

    void ReadFileWithIncludesTMP(const std::string &filename, std::stringstream &sourceBuffer);

    void UpdateProgram(Program &program);

private:
    std::string shaderDirectory{"shaders/"};

    std::unordered_map<std::string, GlslFile> managedFiles;

    std::unordered_map<std::string, ShaderErrorReport> nonCompilingShaders{};

    // Maps from a program name to an index into the publicProgramHandles array
    std::unordered_map<std::string, size_t> managedPrograms{};

    // Maps from a program location to a list of material ID's that need to be reinit on program changes
    std::unordered_map<size_t, std::unordered_set<ShaderDependant *>> dependantObjects{};

    // It is very important that the memory below is never moved or reordered!
    // External pointers point to elements in in this array
    std::array<GLuint, MAX_NUM_MANAGED_PROGRAMS> publicProgramHandles{};

    size_t nextPublicHandleIndex = 0;
};


#endif //OPENGLFRAMEWORK_SHADERSYSTEM_H
