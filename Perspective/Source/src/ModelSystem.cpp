//
// Created by user on 1/25/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: ModelSystem.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/25/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "ModelSystem.h"
#include "Logger.h"
#include "Model.h"
#include "ShaderLocations.h"
#include "TransformSystem.h"
#include "MaterialSystem.h"

#pragma region Internal API

#ifdef TINYOBJLOADER
static void ReadObjShape(LoadedModel& model, tinyobj::shape_t& shape, const std::string& filename, const std::string& baseDirectory,
        const tinyobj::attrib_t& attributes, const std::vector<tinyobj::material_t> materials)
{
    model.filename = filename;
    model.name = shape.name;

    int firstMaterialIndex = shape.mesh.material_ids[0];
    for (int i = 1; i < shape.mesh.material_ids.size(); ++i)
    {
        if (shape.mesh.material_ids[i] != firstMaterialIndex)
        {
            Log("Mesh shape '%s' in '%s' has more than one material defined. Please export mesh with separate material groups! "
                "Only the first will be used, so some things might not look as intended.\n", shape.name.c_str(), filename.c_str());
            break;
        }
    }

    if (firstMaterialIndex != -1)
    {
        model.materialDefined = true;
        model.baseDirectory = baseDirectory;
        model.materialDescription = materials[firstMaterialIndex];
    }
    else
    {
        model.materialDefined = false;
    }

    size_t numInputIndices = shape.mesh.indices.size();
    assert(numInputIndices % 3 == 0);

    // The number of indices will not change
    model.indices.reserve(numInputIndices);
    // There will be no more vertices than indices (but probably and hopefully less!)
    model.vertices.reserve(numInputIndices);

    int numMissingNormals = 0;
    int numMissingTexCoords = 0;

    glm::vec3 minVertex{ INFINITY };
    glm::vec3 maxVertex{ -INFINITY };

    std::unordered_map<uint64_t, uint32_t> indexMap;

    for (size_t i = 0; i < numInputIndices; ++i)
    {
        tinyobj::index_t index = shape.mesh.indices[i];
        uint64_t hash = (index.vertex_index * 5723873ULL + index.normal_index) * 39239ULL + index.texcoord_index;

        auto it = indexMap.find(hash);
        if (it != indexMap.end())
        {
            // This exact vertex already exist, push index of that one
            uint32_t singleIndex = it->second;
            model.indices.emplace_back(singleIndex);
        }
        else
        {
            // This exact vertex doesn't exist, so we have to create it

            Vertex v;

            v.position.x = attributes.vertices[3 * index.vertex_index + 0];
            v.position.y = attributes.vertices[3 * index.vertex_index + 1];
            v.position.z = attributes.vertices[3 * index.vertex_index + 2];

            if (index.normal_index != -1)
            {
                v.normal.x = attributes.normals[3 * index.normal_index + 0];
                v.normal.y = attributes.normals[3 * index.normal_index + 1];
                v.normal.z = attributes.normals[3 * index.normal_index + 2];
            }
            else
            {
                numMissingNormals += 1;
            }

            if (index.texcoord_index != -1)
            {
                v.texCoord.s = attributes.texcoords[2 * index.texcoord_index + 0];
                v.texCoord.t = attributes.texcoords[2 * index.texcoord_index + 1];
            }
            else
            {
                numMissingTexCoords += 1;
            }

            v.tangent = { 0, 0, 0, 0 };

            // Grow the bounding box around the vertex if required
            {
                minVertex.x = fmin(minVertex.x, v.position.x);
                minVertex.y = fmin(minVertex.y, v.position.y);
                minVertex.z = fmin(minVertex.z, v.position.z);

                maxVertex.x = fmax(maxVertex.x, v.position.x);
                maxVertex.y = fmax(maxVertex.y, v.position.y);
                maxVertex.z = fmax(maxVertex.z, v.position.z);
            }

            size_t thisIndex = model.vertices.size();
            model.vertices.emplace_back(v);

            assert(thisIndex < UINT32_MAX);
            uint32_t singleIndex = static_cast<uint32_t>(thisIndex);
            model.indices.emplace_back(singleIndex);
            indexMap[hash] = singleIndex;
        }
    }

    // Make a bounding sphere around the mesh
    model.bounds.center = glm::mix(minVertex, maxVertex, 0.5f);
    model.bounds.radius = glm::distance(model.bounds.center, maxVertex);
    model.boundingBox = Shapes::AABB(minVertex, maxVertex, 0);

    // Generate normals (if not already exists) and tangents (if possible)
    assert(model.indices.size() % 3 == 0);

    bool generateNewNormals = numMissingNormals > 0;

    // Reset normals if we need to create new ones
    if (generateNewNormals)
    {
        for (Vertex& vertex : model.vertices)
        {
            vertex.normal = { 0, 0, 0 };
        }
    }

    bool hasTexCoords = numMissingTexCoords == 0;
    bool generateTangents = hasTexCoords;

    size_t numBitangents = (generateTangents) ? model.vertices.size() : 0;
    std::vector<glm::vec3> bitangents{ numBitangents };

    // Construct tangents (if possible) and new normals (if requested)
    uint32_t index = 0;
    for (size_t i = 0; i < model.indices.size(); i += 3, index += 2)
    {
        uint32_t i0 = model.indices[i + 0];
        uint32_t i1 = model.indices[i + 1];
        uint32_t i2 = model.indices[i + 2];

        Vertex& v0 = model.vertices[i0];
        Vertex& v1 = model.vertices[i1];
        Vertex& v2 = model.vertices[i2];

        glm::vec3 e1 = v1.position - v0.position;
        glm::vec3 e2 = v2.position - v0.position;

        if (generateNewNormals)
        {
            glm::vec3 normal = glm::cross(e1, e2);
            v0.normal += normal;
            v1.normal += normal;
            v2.normal += normal;
        }

        if (generateTangents)
        {
            glm::vec2 tex1 = v1.texCoord - v0.texCoord;
            glm::vec2 tex2 = v2.texCoord - v0.texCoord;

            float r = 1.0F / (tex1.s * tex2.t - tex2.s * tex1.t);

            glm::vec3 sDir = {
                    (tex2.t * e1.x - tex1.t * e2.x) * r,
                    (tex2.t * e1.y - tex1.t * e2.y) * r,
                    (tex2.t * e1.z - tex1.t * e2.z) * r
            };
            glm::vec3 tDir = {
                    (tex1.s * e2.x - tex2.s * e1.x) * r,
                    (tex1.s * e2.y - tex2.s * e1.y) * r,
                    (tex1.s * e2.z - tex2.s * e1.z) * r
            };

            v0.tangent += glm::vec4(sDir, 0.0f);
            v1.tangent += glm::vec4(sDir, 0.0f);
            v2.tangent += glm::vec4(sDir, 0.0f);

            // Save bintangent temporarily
            bitangents[i0] += tDir;
            bitangents[i1] += tDir;
            bitangents[i2] += tDir;
        }

        // Generate debug information for vertex normal lines
        {
            Vertex &start = v0;
            Vertex end;
            const float DebugLineDistance = model.bounds.radius / 2.0f;
            end.position = start.position + (start.normal * DebugLineDistance);
            model.debug.vertexNormalVertices.emplace_back(start);
            model.debug.vertexNormalVertices.emplace_back(end);
            model.debug.vertexNormalIndices.emplace_back(index);
            model.debug.vertexNormalIndices.emplace_back(index + 1);
        }

        // Generate debug information for face normal lines
        {
            Vertex startFaceNormal;
            Vertex endFaceNormal;
            const float DebugLineDistance = model.bounds.radius / 2.0f;
            startFaceNormal.position = v0.position + v1.position + v2.position;
            startFaceNormal.position /= 3;

            glm::vec3 normal = v0.normal + v1.normal + v2.normal;
            normal /= 3;

            endFaceNormal.position = startFaceNormal.position + (normal * DebugLineDistance);
            model.debug.faceNormalVertices.emplace_back(startFaceNormal);
            model.debug.faceNormalVertices.emplace_back(endFaceNormal);
            model.debug.faceNormalIndices.emplace_back(index);
            model.debug.faceNormalIndices.emplace_back(index + 1);
        }
    }

    // Normalize new normals and tangents, and set handedness of (bi)tangents
    for (uint32_t i : model.indices)
    {
        Vertex& vertex = model.vertices[i];

        if (generateNewNormals)
        {
            vertex.normal = glm::normalize(vertex.normal);
        }

        if (generateTangents)
        {
            glm::vec3 N = vertex.normal;
            glm::vec3 B = bitangents[i];
            glm::vec3 T = glm::vec3(vertex.tangent);
            T = glm::normalize(T - N * glm::dot(N, T));

            // Calculate handedness and store in w component
            float handedness = (glm::dot(glm::cross(N, T), B) < 0.0f) ? -1.0f : 1.0f;
            vertex.tangent = glm::vec4(T, handedness);
        }
    }
}


#pragma endregion

void ModelSystem::Init() {
    runBackgroundLoop = true;
    backgroundThread = std::thread([this]()
   {
       while (runBackgroundLoop)
       {
           std::string currentFile;
           {
               std::unique_lock<std::mutex> lock(accessMutex);
               while (pendingFiles.IsEmpty() && runBackgroundLoop)
               {
                   runCondition.wait(lock);
               }

               if (!runBackgroundLoop)
               {
                   return;
               }

               currentFile = pendingFiles.Pop();
           }

           auto pathIndex = currentFile.find_last_of('/');
           if (pathIndex == std::string::npos) pathIndex = currentFile.find_last_of('\\');

           std::string baseDirectory = "";
           if (pathIndex != std::string::npos)
           {
               baseDirectory = currentFile.substr(0, pathIndex + 1);
           }

           tinyobj::attrib_t attributes;
           std::vector<tinyobj::shape_t> shapes;
           std::vector<tinyobj::material_t> materials;

           const bool triangulate = true;
           std::string error;
           if (!tinyobj::LoadObj(&attributes, &shapes, &materials, &error, currentFile.c_str(), baseDirectory.c_str(), triangulate))
           {
               Log("Could not load model '%s': %s.\n", currentFile.c_str(), error.c_str());
               currentJobsCounter -= 1;
               continue;
           }

           loadedData[currentFile].resize(shapes.size());

           for (int shapeIdx = 0; shapeIdx < shapes.size(); ++shapeIdx)
           {
               tinyobj::shape_t& shape = shapes[shapeIdx];
               LoadedModel& loadedModel = loadedData[currentFile][shapeIdx];

               ReadObjShape(loadedModel, shape, currentFile, baseDirectory, attributes, materials);
           }

           // Push loaded and processed data for this shape when finished so that Update() can load it to the GPU as soon as possible
           std::lock_guard<std::mutex> lock(accessMutex);
           finishedFiles.Push(currentFile);
       }
   });
}

void ModelSystem::Destroy() {
    runBackgroundLoop = false;
    runCondition.notify_all();
    backgroundThread.join();
}

void ModelSystem::Update() {
    // If it's empty don't even bother to try to get a lock etc. Most frames there won't
    // be anything here, so this makes sure we don't waste precious frame time.
    if (finishedFiles.IsEmpty())
    {
        return;
    }

    // If there definitely is at least one finished job, aqcuire a lock
    std::lock_guard<std::mutex> lock(accessMutex);

    while (!finishedFiles.IsEmpty())
    {
        std::string filename = finishedFiles.Pop();
        const std::vector<LoadedModel>& loadedModels = loadedData[filename];

        std::vector<Model> models;
        models.reserve(loadedModels.size());

        for (int modelIdx = 0; modelIdx < loadedModels.size(); ++modelIdx)
        {
            const LoadedModel& loadedModel = loadedModels[modelIdx];

            if (loadedModel.indices.size() <= 0 || loadedModel.vertices.size() <= 0)
            {
                Log("Ignoring model since it has either no indices or no vertices defined!\n");
                currentJobsCounter -= 1;
                continue;
            }
            Model model = RegisterModel(loadedModel);
            models.emplace_back(model);
        }

        auto callback = callbackForFile[filename];
        callback(models);

        currentJobsCounter -= 1;
    }
}



bool ModelSystem::IsIdle() {
    assert(currentJobsCounter >= 0);
    return currentJobsCounter == 0;
}

void ModelSystem::LoadModel(const std::string &filename, const ModelSystem::ModelLoadCallback &callback) {
    currentJobsCounter += 1;

    if (loadedData.find(filename) != loadedData.end())
    {
        // File is already loaded. Immediately push the models to the done queue so that
        // the Update() method will notice it and create GPU represenations for it

        std::lock_guard<std::mutex> lock(accessMutex);

        callbackForFile[filename] = callback;
        finishedFiles.Push(filename);
    }
    else
    {
        std::lock_guard<std::mutex> lock(accessMutex);

        callbackForFile[filename] = callback;
        pendingFiles.Push(filename);

        runCondition.notify_all();
    }
}

const std::unordered_map<std::string,std::vector<LoadedModel>> &ModelSystem::GetAllLoadedModels() const {
    return this->loadedData;
}

Model ModelSystem::RegisterModel(const LoadedModel &loadedModel) {


    GLuint indexBuffer;
    GLsizei indexCount;
    GLenum  indexType;
    {
        glCreateBuffers(1, &indexBuffer);

        const auto& data = loadedModel.indices;

        indexCount = static_cast<GLsizei>(data.size());
        indexType = GL_UNSIGNED_INT;

        size_t size = sizeof(uint32_t) * indexCount;

        GLbitfield flags = GL_DYNAMIC_STORAGE_BIT; // TODO: Consider these! Good default?
        glNamedBufferStorage(indexBuffer, size, data.data(), flags);
    }

    GLuint vertexBuffer;
    {
        glCreateBuffers(1, &vertexBuffer);

        const auto& data = loadedModel.vertices;
        size_t size = sizeof(Vertex) * data.size();
        GLbitfield flags = GL_DYNAMIC_STORAGE_BIT; // TODO: Consider these! Good default?
        glNamedBufferStorage(vertexBuffer, size, data.data(), flags);
    }

    GLuint vao;
    glCreateVertexArrays(1, &vao);

    // Specify the element buffer for this vertex array
    glVertexArrayElementBuffer(vao, indexBuffer);

    // Bind the vertex array to a specific binding index and specify it stride, etc.
    GLuint vertexArrayBindingIndex = 0;
    glVertexArrayVertexBuffer(vao, vertexArrayBindingIndex, vertexBuffer, 0, sizeof(Vertex));

    // Enable the attribute, specify its format, and connect the vertex array (at its
    // binding index) to to this specific attribute for this vertex array
    glEnableVertexArrayAttrib(vao, PredefinedAttributeLocation(a_position));
    glVertexArrayAttribFormat(vao, PredefinedAttributeLocation(a_position), 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
    glVertexArrayAttribBinding(vao, PredefinedAttributeLocation(a_position), vertexArrayBindingIndex);

    glEnableVertexArrayAttrib(vao, PredefinedAttributeLocation(a_normal));
    glVertexArrayAttribFormat(vao, PredefinedAttributeLocation(a_normal), 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
    glVertexArrayAttribBinding(vao, PredefinedAttributeLocation(a_normal), vertexArrayBindingIndex);

    glEnableVertexArrayAttrib(vao, PredefinedAttributeLocation(a_tex_coord));
    glVertexArrayAttribFormat(vao, PredefinedAttributeLocation(a_tex_coord), 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texCoord));
    glVertexArrayAttribBinding(vao, PredefinedAttributeLocation(a_tex_coord), vertexArrayBindingIndex);

    glEnableVertexArrayAttrib(vao, PredefinedAttributeLocation(a_tangent));
    glVertexArrayAttribFormat(vao, PredefinedAttributeLocation(a_tangent), 4, GL_FLOAT, GL_FALSE, offsetof(Vertex, tangent));
    glVertexArrayAttribBinding(vao, PredefinedAttributeLocation(a_tangent), vertexArrayBindingIndex);

    DebugModel vertexNormalDebug;
    // Debug Stuff
    {
        // Vertex Normals
        {
            GLuint vertexNormalIndexBuffer;
            GLsizei vertexNormalIndexCount;
            GLenum vertexNormalIndexType;
            {
                glCreateBuffers(1, &vertexNormalIndexBuffer);

                const auto &data = loadedModel.debug.vertexNormalIndices;

                vertexNormalIndexCount = static_cast<GLsizei>(data.size());
                vertexNormalIndexType = GL_UNSIGNED_INT;

                size_t size = sizeof(uint32_t) * vertexNormalIndexCount;

                GLbitfield flags = GL_DYNAMIC_STORAGE_BIT; // TODO: Consider these! Good default?
                glNamedBufferStorage(vertexNormalIndexBuffer, size, data.data(), flags);
            }

            GLuint vertexNormalBuffer;
            {
                glCreateBuffers(1, &vertexNormalBuffer);

                const auto &data = loadedModel.debug.vertexNormalVertices;
                size_t size = sizeof(Vertex) * data.size();
                GLbitfield flags = GL_DYNAMIC_STORAGE_BIT; // TODO: Consider these! Good default?
                glNamedBufferStorage(vertexNormalBuffer, size, data.data(), flags);
            }

            GLuint vertexNormalVao;
            {
                glCreateVertexArrays(1, &vertexNormalVao);

                // Specify the element buffer for this vertex array
                glVertexArrayElementBuffer(vertexNormalVao, vertexNormalIndexBuffer);

                // Bind the vertex array to a specific binding index and specify it stride, etc.
                GLuint vertexArrayBindingIndex = 0;
                glVertexArrayVertexBuffer(vertexNormalVao, vertexArrayBindingIndex, vertexNormalBuffer, 0,
                                          sizeof(Vertex));

                // Enable the attribute, specify its format, and connect the vertex array (at its
                // binding index) to to this specific attribute for this vertex array
                glEnableVertexArrayAttrib(vertexNormalVao, PredefinedAttributeLocation(a_position));
                glVertexArrayAttribFormat(vertexNormalVao, PredefinedAttributeLocation(a_position), 3, GL_FLOAT,
                                          GL_FALSE,
                                          offsetof(Vertex, position));
                glVertexArrayAttribBinding(vertexNormalVao, PredefinedAttributeLocation(a_position),
                                           vertexArrayBindingIndex);
            }
            vertexNormalDebug.vao = vertexNormalVao;
            vertexNormalDebug.indexCount = vertexNormalIndexCount;
            vertexNormalDebug.indexType = vertexNormalIndexType;
        }
    }
    DebugModel faceNormalDebug;
    {
        // Face Normals
        {
            GLuint facerNormalIndexBuffer;
            GLsizei faceNormalIndexCount;
            GLenum  faceNormalIndexType;
            {
                glCreateBuffers(1, &facerNormalIndexBuffer);

                const auto& data = loadedModel.debug.faceNormalIndices;

                faceNormalIndexCount = static_cast<GLsizei>(data.size());
                faceNormalIndexType = GL_UNSIGNED_INT;

                size_t size = sizeof(uint32_t) * faceNormalIndexCount;

                GLbitfield flags = GL_DYNAMIC_STORAGE_BIT; // TODO: Consider these! Good default?
                glNamedBufferStorage(facerNormalIndexBuffer, size, data.data(), flags);
            }

            GLuint faceNormalVertexBuffer;
            {
                glCreateBuffers(1, &faceNormalVertexBuffer);

                const auto& data = loadedModel.debug.faceNormalVertices;
                size_t size = sizeof(Vertex) * data.size();
                GLbitfield flags = GL_DYNAMIC_STORAGE_BIT; // TODO: Consider these! Good default?
                glNamedBufferStorage(faceNormalVertexBuffer, size, data.data(), flags);
            }

            GLuint faceNormalVao;
            {
                glCreateVertexArrays(1, &faceNormalVao);

                // Specify the element buffer for this vertex array
                glVertexArrayElementBuffer(faceNormalVao, facerNormalIndexBuffer);

                // Bind the vertex array to a specific binding index and specify it stride, etc.
                GLuint vertexArrayBindingIndex = 0;
                glVertexArrayVertexBuffer(faceNormalVao, vertexArrayBindingIndex, faceNormalVertexBuffer, 0, sizeof(Vertex));

                // Enable the attribute, specify its format, and connect the vertex array (at its
                // binding index) to to this specific attribute for this vertex array
                glEnableVertexArrayAttrib(faceNormalVao, PredefinedAttributeLocation(a_position));
                glVertexArrayAttribFormat(faceNormalVao, PredefinedAttributeLocation(a_position), 3, GL_FLOAT, GL_FALSE,
                                          offsetof(Vertex, position));
                glVertexArrayAttribBinding(faceNormalVao, PredefinedAttributeLocation(a_position),
                                           vertexArrayBindingIndex);
            }
            faceNormalDebug.vao = faceNormalVao;
            faceNormalDebug.indexCount = faceNormalIndexCount;
            faceNormalDebug.indexType = faceNormalIndexType;
        }
    }

    Model model;
    model.vao = vao;
    model.indexCount = indexCount;
    model.indexType = indexType;

    model.faceNormal = faceNormalDebug;
    model.vertexNormal = vertexNormalDebug;

    model.bounds = loadedModel.bounds;
    model.boundingBox = loadedModel.boundingBox;
    model.transformID = TransformSystem::getInstance().Create();
    model.faceNormal.transformID = model.transformID;
    model.vertexNormal.transformID = model.transformID;

    // Register/create material (must be done here on the main thread!)
    if (loadedModel.materialDefined)
    {
        model.material = MaterialSystem::getInstance().CreateMaterial(loadedModel.materialDescription, loadedModel.baseDirectory);
    }
    model.name = loadedModel.filename;
    model.loadedModel = &loadedModel;
    return model;
}

#elif ASSIMPLOADER

void ModelSystem::Init() {}

void ModelSystem::Destroy() {}

void ModelSystem::Update() {}

#endif