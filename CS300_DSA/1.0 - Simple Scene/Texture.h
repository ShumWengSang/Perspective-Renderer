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
    enum STBI_COMP_TYPE
    {
        STBI_default = 0, // only used for desired_channels

        STBI_grey       = 1,
        STBI_grey_alpha = 2,
        STBI_rgb        = 3,
        STBI_rgb_alpha  = 4
    };

    Texture(GLuint textureID, GLuint bindingPort);
    ~Texture();
    using stb_comp_t = STBI_COMP_TYPE;
    static GLuint CreateTexture2D(GLenum internalFormat, GLenum format, GLsizei  width, GLsizei  height,
                                  void* data = nullptr, GLenum filter = GL_LINEAR, GLenum repeat = GL_REPEAT);
    static GLuint CreateTextureCube(std::array<std::string_view, 6> const & filepath, stb_comp_t comp = STBI_rgb_alpha);
    static GLuint CreateTexture2DFromFile(std::string_view filepath, stb_comp_t comp = STBI_rgb_alpha);
    static std::tuple<GLint,GLint,GLint,GLint,GLint,GLint> CreateTextureCubeCustom(std::vector<std::string> const & str);

    void Destroy();
    void Bind();

    GLuint GetID() const;
private:
    GLuint textureID;
    GLuint bindingPort;
};


#endif //SIMPLE_SCENE_TEXTURE_H
