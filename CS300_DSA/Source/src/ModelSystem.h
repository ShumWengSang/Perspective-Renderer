//
// Created by user on 1/25/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: ModelSystem.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/25/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_MODELSYSTEM_H
#define OPENGLFRAMEWORK_MODELSYSTEM_H
#include "Shapes.h"
#pragma region Internal Structures
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec4 tangent; // (w is bitangent's handedness)
};

struct LoadedModel
{
    std::string filename;
    std::string name;

    std::vector<uint32_t> indices;
    std::vector<Vertex> vertices;

    Shapes::BoundingSphere bounds;

    bool materialDefined;
    std::string baseDirectory;
    tinyobj::material_t materialDescription;
};
#pragma endregion


class Model;

class ModelSystem {
    // Singleton stuff
public:
    static ModelSystem& getInstance()
    {
        static ModelSystem instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    ModelSystem(ModelSystem const&) = delete;
    void operator=(ModelSystem const&)  = delete;
private:
    ModelSystem() = default;

public:
    void Init();
    void Destroy();
    void Update();
    bool IsIdle();
    using ModelLoadCallback = std::function<void(std::vector<Model> models)>;
    void LoadModel(const std::string& filename, const ModelLoadCallback& callback);

    const std::unordered_map<std::string,std::vector<LoadedModel>>& GetAllLoadedModels() const;

private:
    Queue<std::string> pendingFiles{};
    Queue<std::string> finishedFiles{};

    std::unordered_map<std::string, std::vector<LoadedModel>> loadedData{};

    // TODO/FIXME: what if we make two calls for the same file with different callbacks?!
    std::unordered_map<std::string, ModelSystem::ModelLoadCallback> callbackForFile{};

    std::atomic_int currentJobsCounter;

    std::thread             backgroundThread;
    std::mutex              accessMutex;
    std::condition_variable runCondition;
    bool                    runBackgroundLoop;
};


#endif //OPENGLFRAMEWORK_MODELSYSTEM_H
