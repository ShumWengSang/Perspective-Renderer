//
// Created by user on 1/24/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: TransformSystem.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/24/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_TRANSFORMSYSTEM_H
#define OPENGLFRAMEWORK_TRANSFORMSYSTEM_H

#define MAX_NUM_TRANSFORMS 1024 * 1024

class Transform {
public:
    Transform() = default;
    ~Transform() = default;

    glm::vec3 position{};
    glm::quat orientation{ 1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale{ 1.0f };

    glm::mat4 matrix{ 1.0f };
    glm::mat4 inverseMatrix{ 1.0f };
    glm::mat3 normalMatrix{ 1.0f };

    Transform& SetPosition(float x, float y, float z);
    Transform& SetScale(float s);
    Transform& SetScale(float x, float y, float z);
    Transform& SetDirection(float x, float y, float z);
};

class TransformSystem {
public:
    void Init();

    // Must be called before every frame
    void Update();

    void Destroy();

    int Create();
    Transform& Get(int transformID);
    const Transform& GetPrevious(int transformID);

    void UpdateMatrices(int transformID);

    static TransformSystem& getInstance()
    {
        static TransformSystem instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    TransformSystem(TransformSystem const&) = delete;
    void operator=(TransformSystem const&)  = delete;
private:
    TransformSystem() = default;

    inline static const glm::mat4 identity { 1.0f };
    inline static int nextIndex = 0;
    inline static std::array<Transform, MAX_NUM_TRANSFORMS> transforms;
    inline static std::array<Transform, MAX_NUM_TRANSFORMS> oldTransforms;
};


#endif //OPENGLFRAMEWORK_TRANSFORMSYSTEM_H
