//
// Created by user on 11/8/2020.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: Texture.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Simple_Scene
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 11/8/2020
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


GLuint Texture::CreateTexture2D(
        GLenum internalFormat, GLenum format, GLsizei width, GLsizei height, void *data, GLenum filter, GLenum repeat
                               ) {
    GLuint tex = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &tex);
    glTextureStorage2D(tex, 1, internalFormat, width, height);

    glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, filter);
    glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, filter);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_S, repeat);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_T, repeat);
    glTextureParameteri(tex, GL_TEXTURE_WRAP_R, repeat);

    if (data)
    {
        glTextureSubImage2D(tex, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
    }

    return tex;
}

GLuint Texture::CreateTexture2DFromFile(std::string_view filepath, Texture::stb_comp_t comp) {
    int x, y, c;
    if (!std::filesystem::exists(filepath.data()))
    {
        std::ostringstream message;
        message << "file " << filepath.data() << " does not exist.";
        throw std::runtime_error(message.str());
    }
    const auto data = stbi_load(filepath.data(), &x, &y, &c, comp);

    if(data == nullptr)
    {
        std::cout << "Load image error: " << stbi_failure_reason() << std::endl;
        DEBUG_BREAKPOINT();
    }

    auto const[in, ex] = [comp]() {
        switch (comp)
        {
            case STBI_rgb_alpha:	return std::make_pair(GL_RGBA8, GL_RGBA);
            case STBI_rgb:			return std::make_pair(GL_RGB8, GL_RGB);
            case STBI_grey:			return std::make_pair(GL_R8, GL_RED);
            case STBI_grey_alpha:	return std::make_pair(GL_RG8, GL_RG);
            default: throw std::runtime_error("invalid format");
        }
    }();

    const auto name = CreateTexture2D(in, ex, x, y, data);
    stbi_image_free(data);
    return name;
}

Texture::Texture(GLuint textureID, GLuint bindingPort) : textureID(textureID), bindingPort(bindingPort)
{

}

Texture::~Texture()
{
    Destroy();
}

void Texture::Destroy()
{
    glDeleteTextures(1, &textureID);
}

void Texture::Bind()
{
    glBindTextureUnit(bindingPort, textureID);
}