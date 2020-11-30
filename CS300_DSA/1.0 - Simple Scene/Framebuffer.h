//
// Created by user on 11/28/2020.
//


/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: Framebuffer.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Simple_Scene
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 11/28/2020
 * End Header --------------------------------------------------------*/

#ifndef SIMPLE_SCENE_FRAMEBUFFER_H
#define SIMPLE_SCENE_FRAMEBUFFER_H

class Framebuffer {
public:
    static GLuint CreateFrameBuffer(std::vector<GLuint> const & cols, GLuint depth = GL_NONE);
    explicit Framebuffer(GLuint id);
    Framebuffer(Framebuffer const &) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    ~Framebuffer();

    void Bind() const;
    static void Unbind();
    void Clear(GLenum buffer, GLint drawBuffer, const GLfloat * value);
    GLuint GetID() const;
private:
    GLuint framebufferID;

};


#endif //SIMPLE_SCENE_FRAMEBUFFER_H
