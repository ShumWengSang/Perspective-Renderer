//
// Created by user on 1/25/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: ShaderSystem.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/25/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "ShaderSystem.h"
#include "ShaderDependant.h"
#include "Logger.h"
#include "GuiSystem.h"

#pragma region Internal API

#ifndef _WIN32
template <typename TP>
std::time_t to_time_t(TP tp)
{
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
                                                        + system_clock::now());
    return system_clock::to_time_t(sctp);
}
#endif

void ShaderSystem::AddManagedFile(const std::string &filename, const Program &dependableProgram) {
    if (managedFiles.find(filename) == managedFiles.end()) {
        managedFiles[filename] = GlslFile(filename);
    }

    managedFiles[filename].dependablePrograms.emplace(dependableProgram);
}

std::time_t ShaderSystem::GetTimestamp(const GlslFile &file) const {
    namespace fs = std::filesystem;
    auto filename = shaderDirectory + file.filename;

#if _WIN32
    struct __stat64 stFileInfo;
    if (_stat64(filename.c_str(), &stFileInfo) != 0) {
        throw std::runtime_error("Failed to get last write time.");
    }
    return stFileInfo.st_mtime;
#else
    auto fsTime = std::filesystem::last_write_time ( filename );
    return to_time_t ( fsTime );
#endif

}

bool ShaderSystem::FileReadable(const std::string &filename) const {
    auto path = shaderDirectory + filename;
    std::ifstream ifs(path);
    return ifs.good();
}

void ShaderSystem::ReadFileWithIncludes(
        const std::string &filename, const Program &dependableProgram, std::stringstream &sourceBuffer
                                       ) {
    // (won't do anything if the file is already managed)
    AddManagedFile(filename, dependableProgram);

    auto path = shaderDirectory + filename;
    std::ifstream ifs(path);
    if (!ifs.good() || ifs.fail()) {
        Log("Could not read shader file '%s'.\n", filename.c_str());
    }

    std::string line;
    while (std::getline(ifs, line)) {
        size_t commentIndex = line.find("//");
        size_t index = line.find("#include");

        if (index == -1 || (commentIndex < index && commentIndex != -1)) {
            sourceBuffer << line;
        } else {
            // This isn't very precise but it should work assuming that
            // the programmer isn't doing anything really stupid.
            size_t start = line.find('<') + 1;
            size_t end = line.find('>');

            int count = int(end) - int(start);
            if (count < 0) {
                LogError("Invalid include directive: %s\n", line.c_str());
            }

            std::string includeFile = line.substr(start, count);
            ReadFileWithIncludes(includeFile, dependableProgram, sourceBuffer);
        }

        sourceBuffer << '\n';
    }
}

void ShaderSystem::ReadFileWithIncludesTMP(const std::string &filename, std::stringstream &sourceBuffer) {
    auto path = shaderDirectory + filename;
    std::ifstream ifs(path);
    if (!ifs.good()) {
        Log("Could not read shader file '%s'.\n", filename.c_str());
    }

    std::string line;
    while (std::getline(ifs, line)) {
        size_t commentIndex = line.find("//");
        size_t index = line.find("#include");

        if (index == -1 || (commentIndex < index && commentIndex != -1)) {
            sourceBuffer << line;
        } else {
            // This isn't very precise but it should work assuming that
            // the programmer isn't doing anything really stupid.
            size_t start = line.find('<') + 1;
            size_t end = line.find('>');

            int count = int(end) - int(start);
            if (count < 0) {
                LogError("Invalid include directive: %s\n", line.c_str());
            }

            std::string includeFile = line.substr(start, count);
            ReadFileWithIncludesTMP(includeFile, sourceBuffer);
        }

        sourceBuffer << '\n';
    }
}

void ShaderSystem::UpdateProgram(Program &program) {
    static GLchar statusBuffer[4096];
    bool oneOrMoreShadersFailedToCompile = false;

    GLuint programHandle = glCreateProgram();
    std::vector<GLuint> shaderHandles{};

    for (auto &shader: program.shaders) {
        std::stringstream sourceBuffer{};
        ReadFileWithIncludes(shader.filename, program, sourceBuffer);

        GLuint shaderHandle = glCreateShader(shader.type);

        std::string source = sourceBuffer.str();
        const GLchar *sources[] = {source.c_str()};
        glShaderSource(shaderHandle, 1, sources, nullptr);
        glCompileShader(shaderHandle);

        GLint compilationSuccess;
        glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compilationSuccess);
        if (compilationSuccess != GL_TRUE) {
            oneOrMoreShadersFailedToCompile = true;
            glGetShaderInfoLog(shaderHandle, sizeof(statusBuffer), nullptr, statusBuffer);
            Log("Shader compilation error ('%s'): %s\n", shader.filename.c_str(), statusBuffer);

            ShaderErrorReport report;
            {
                int lineNum = 1;
                std::stringstream textWithLineNrs;

                for (std::string line; std::getline(sourceBuffer, line); ++lineNum) {
                    textWithLineNrs << std::setfill(' ') << std::setw(3) << lineNum << "  " << line << std::endl;
                }

                report.preprocessedSource = textWithLineNrs.str();
                report.errorMessage = std::string(statusBuffer);
                report.shaderName = shader.filename;
            }
            nonCompilingShaders[shader.filename] = report;
        } else {
            glAttachShader(programHandle, shaderHandle);
            shaderHandles.push_back(shaderHandle);

            // In case it previously haven't compiled, now remove it from the list
            nonCompilingShaders.erase(shader.filename);
        }
    }

    glLinkProgram(programHandle);

    // (it's safe to detach and delete shaders after linking)
    for (GLuint shaderHandle: shaderHandles) {
        glDetachShader(programHandle, shaderHandle);
        glDeleteShader(shaderHandle);
    }

    if (oneOrMoreShadersFailedToCompile) {
        return;
    }

    GLint linkSuccess;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccess);

    if (linkSuccess != GL_TRUE) {
        glGetProgramInfoLog(programHandle, sizeof(statusBuffer), nullptr, statusBuffer);
        Log("Shader program link error: %s\n", statusBuffer);
    } else {
        // The program successfully compiled and linked, it's safe to replace the old one

        size_t index = program.fixedLocation;

        GLuint oldProgramHandle = publicProgramHandles[index];
        if (oldProgramHandle) {
            glDeleteProgram(oldProgramHandle);
        }

        publicProgramHandles[index] = programHandle;

        // Notify all dependant objects
        for (auto shaderDependant: dependantObjects[program.fixedLocation]) {
            if (shaderDependant) {
                shaderDependant->ProgramLoaded(programHandle);
            }
        }
    }
}

#pragma endregion

void ShaderSystem::Update(float dt) {
    static GLchar statusBuffer[4096];
    static float timer = 0;
    timer += dt;

    // Now update GUI
    auto GuiFunc = [&]() -> void {
        auto reports = ShaderSystem::getInstance().GetShaderErrorReports();
        for (auto& report : reports) {
            ImGui::Begin(("Shader error report for '" + report.shaderName + "'").c_str());
            {
                ImGui::Text(report.shaderName.c_str());
                ImGui::TextColored({ 1, 0, 0, 1 }, report.errorMessage.c_str());

                // NOTE: We will always use the read-only flag here, so this const cast should be perfectly fine!
                char* data = const_cast<char*>(report.preprocessedSource.data());
                size_t dataSize = report.preprocessedSource.size();

                // Give a reasonable size the first time a file shows up
                ImGui::SetWindowSize({ 800, 600 }, ImGuiCond_Once);

                // (this size seems to work fine for fitting all relevant data)
                ImVec2 panelSize = ImGui::GetWindowSize();
                panelSize.y -= 50 + ImGui::CalcTextSize(report.errorMessage.c_str()).y +
                    ImGui::CalcTextSize(report.shaderName.c_str()).y;
                panelSize.x -= 15;

                ImGui::InputTextMultiline("", data, dataSize, panelSize, ImGuiInputTextFlags_ReadOnly);
            }
            ImGui::End();
        }
    };

    GuiSystem& guiSystem = GuiSystem::getInstance();

    guiSystem.AddGui("Shader Error Reports", std::ref(GuiFunc));

    if(timer <= 1.0f)
        return;
    timer = 0.f;

    std::unordered_set<Program> programsToUpdate{};

    for (auto &pair: managedFiles) {
        GlslFile &file = pair.second;

        uint64_t timestamp = GetTimestamp(file);
        if (timestamp != file.timestamp) {
            file.timestamp = timestamp;

            // Add all programs that are dependant of this file (directly or included)
            programsToUpdate.insert(file.dependablePrograms.begin(), file.dependablePrograms.end());
        }
    }

    for (auto program: programsToUpdate) {
        UpdateProgram(program);
    }


}

std::vector<ShaderErrorReport> ShaderSystem::GetShaderErrorReports() {
    std::vector<ShaderErrorReport> reports;
    for (auto &pair: nonCompilingShaders) {
        reports.emplace_back(pair.second);
    }

    return reports;
}

GLuint *ShaderSystem::AddProgram(const std::string &name, ShaderDependant *shaderDependant) {
    return AddProgram(name + ".vert.glsl", name + ".frag.glsl", shaderDependant);
}

GLuint *
ShaderSystem::AddProgram(const std::string &vertName, const std::string &fragName, ShaderDependant *shaderDependant) {
    std::string fullName = vertName + "_" + fragName;
    if (managedPrograms.find(fullName) == managedPrograms.end()) {
        Program program;
        program.fixedLocation = nextPublicHandleIndex++;

        Shader vertexShader(GL_VERTEX_SHADER, vertName);
        program.shaders.push_back(vertexShader);

        // Since there can be programs without fragment shaders, consider it optional
        bool includeFragShader = FileReadable(fragName);
        if (includeFragShader) {
            Shader fragmentShader(GL_FRAGMENT_SHADER, fragName);
            program.shaders.push_back(fragmentShader);
        }

        AddManagedFile(vertName, program);
        if (includeFragShader) AddManagedFile(fragName, program);

        if (shaderDependant) {
            dependantObjects[program.fixedLocation].emplace(shaderDependant);
        }

        // Trigger the initial load
        UpdateProgram(program);

        managedPrograms[fullName] = program.fixedLocation;
        return &publicProgramHandles[program.fixedLocation];
    } else {
        // If this exact program is already added (i.e. same vert & frag names), return that address instead
        size_t fixedLocation = managedPrograms[fullName];

        if (shaderDependant) {
            dependantObjects[fixedLocation].emplace(shaderDependant);

            // Since this exact program is added previously there is a chance that it's already loaded.
            // If it is, call program loaded immediately so that it can perform its initial setup.
            GLuint program = publicProgramHandles[fixedLocation];
            if (program) {
                shaderDependant->ProgramLoaded(program);
            }

        }

        return &publicProgramHandles[fixedLocation];
    }
}

void ShaderSystem::AddProgram(
        GLuint **programOut, const std::string &vertName, const std::string &fragName, ShaderDependant *shaderDependant
                             ) {
    std::string fullName = vertName + "_" + fragName;
    if (managedPrograms.find(fullName) == managedPrograms.end()) {
        Program program;
        program.fixedLocation = nextPublicHandleIndex++;

        Shader vertexShader(GL_VERTEX_SHADER, vertName);
        program.shaders.push_back(vertexShader);

        // Since there can be programs without fragment shaders, consider it optional
        bool includeFragShader = FileReadable(fragName);
        if (includeFragShader) {
            Shader fragmentShader(GL_FRAGMENT_SHADER, fragName);
            program.shaders.push_back(fragmentShader);
        }

        AddManagedFile(vertName, program);
        if (includeFragShader) AddManagedFile(fragName, program);

        if (shaderDependant) {
            dependantObjects[program.fixedLocation].emplace(shaderDependant);
        }

        managedPrograms[fullName] = program.fixedLocation;
        *programOut = &publicProgramHandles[program.fixedLocation];

        // Trigger the initial load
        UpdateProgram(program);
    } else {
        // If this exact program is already added (i.e. same vert & frag names), return that address instead
        size_t fixedLocation = managedPrograms[fullName];
        *programOut = &publicProgramHandles[fixedLocation];

        if (shaderDependant) {
            dependantObjects[fixedLocation].emplace(shaderDependant);

            // Since this exact program is added previously there is a chance that it's already loaded.
            // If it is, call program loaded immediately so that it can perform its initial setup.
            GLuint program = publicProgramHandles[fixedLocation];
            if (program) {
                shaderDependant->ProgramLoaded(program);
            }
        }
    }
}

void ShaderSystem::AddComputeProgram(GLuint **programOut, const std::string &name, ShaderDependant *shaderDependant) {
    if (managedPrograms.find(name) == managedPrograms.end()) {
        Program program;
        program.fixedLocation = nextPublicHandleIndex++;

        Shader shader(GL_COMPUTE_SHADER, name);
        program.shaders.push_back(shader);

        AddManagedFile(name, program);

        if (shaderDependant) {
            dependantObjects[program.fixedLocation].emplace(shaderDependant);
        }

        managedPrograms[name] = program.fixedLocation;
        *programOut = &publicProgramHandles[program.fixedLocation];

        // Trigger the initial load
        UpdateProgram(program);
    } else {
        size_t fixedLocation = managedPrograms[name];
        *programOut = &publicProgramHandles[fixedLocation];

        if (shaderDependant) {
            dependantObjects[fixedLocation].emplace(shaderDependant);

            // Since this exact program is added previously there is a chance that it's already loaded.
            // If it is, call program loaded immediately so that it can perform its initial setup.
            GLuint program = publicProgramHandles[fixedLocation];
            if (program) {
                shaderDependant->ProgramLoaded(program);
            }
        }
    }
}

GLuint *ShaderSystem::AddComputeProgram(const std::string &name, ShaderDependant *shaderDependant) {
    if (managedPrograms.find(name) == managedPrograms.end()) {
        Program program;
        program.fixedLocation = nextPublicHandleIndex++;

        Shader shader(GL_COMPUTE_SHADER, name);
        program.shaders.push_back(shader);

        AddManagedFile(name, program);

        if (shaderDependant) {
            dependantObjects[program.fixedLocation].emplace(shaderDependant);
        }

        // Trigger the initial load
        UpdateProgram(program);

        managedPrograms[name] = program.fixedLocation;
        return &publicProgramHandles[program.fixedLocation];
    } else {
        size_t fixedLocation = managedPrograms[name];

        if (shaderDependant) {
            dependantObjects[fixedLocation].emplace(shaderDependant);

            // Since this exact program is added previously there is a chance that it's already loaded.
            // If it is, call program loaded immediately so that it can perform its initial setup.
            GLuint program = publicProgramHandles[fixedLocation];
            if (program) {
                shaderDependant->ProgramLoaded(program);
            }
        }

        return &publicProgramHandles[fixedLocation];
    }
}
