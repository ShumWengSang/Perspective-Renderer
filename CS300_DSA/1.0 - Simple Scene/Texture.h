//
// Created by user on 11/8/2020.
//

//
// Created by user on 11/8/2020.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: Texture.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Simple_Scene
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 11/8/2020
 * End Header --------------------------------------------------------*/


#ifndef SIMPLE_SCENE_TEXTURE_H
#define SIMPLE_SCENE_TEXTURE_H


class Texture {
public:
    Texture();
    ~Texture();

    static GLuint CreateTexture2D(GLenum internalFormat, GLenum format, GLsizei  width, GLsizei  height,
                                  void* data = nullptr, GLenum filter = GL_LINEAR, GLenum repeat = GL_REPEAT)
    {
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
};


#endif //SIMPLE_SCENE_TEXTURE_H
