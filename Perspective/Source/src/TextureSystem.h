//
// Created by user on 1/25/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: TextureSystem.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/25/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_TEXTURESYSTEM_H
#define OPENGLFRAMEWORK_TEXTURESYSTEM_H

#pragma region InternalStructures
struct ImageLoadDescription {
    std::string filename;
    GLuint texture;
    GLenum format, internalFormat;
    bool requestMipmaps;
    bool isHdr;
};

struct LoadedImage {
    void *pixels;
    GLenum type;
    int width, height;
};
#pragma endregion


class TextureSystem {
    // Singleton stuff
public:
    static TextureSystem &getInstance() {
        static TextureSystem instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    TextureSystem(TextureSystem const &) = delete;

    void operator=(TextureSystem const &) = delete;

private:
    TextureSystem() = default;

public:
    void Init();

    void Destroy();

    // Must be called on a regular basis (e.g. in the beginning of every frame)
    void Update();

    bool IsIdle();

    bool IsHdrFile(const std::string &filename);

    GLuint CreatePlaceholder(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF);

    GLuint CreateTexture(
            int width, int height, GLenum format, GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR
            , GLenum magFilter = GL_LINEAR, bool useMips = true
                        );

    GLuint LoadDataTexture(const std::string &filename, GLenum internalFormat = GL_RGBA8);

    // We handle cubes differently, we assume user is smart.
    static GLuint LoadCubeMap(const std::array<std::string, 6> &fileNames, GLenum internalFormat = GL_RGBA8);

private:

    std::unordered_map<std::string, LoadedImage> loadedImages{};
    Queue<ImageLoadDescription> pendingJobs{};
    Queue<ImageLoadDescription> finishedJobs{};

    std::atomic_int currentJobsCounter;

    std::thread backgroundThread;
    std::mutex accessMutex;
    std::condition_variable runCondition;
    bool runBackgroundLoop;

private:
    static GLuint CreateTextureCube(
            GLenum internal_format, GLenum format, GLsizei width, GLsizei height, std::array<stbi_uc *, 6> const &data
                                   );

};


#endif //OPENGLFRAMEWORK_TEXTURESYSTEM_H
