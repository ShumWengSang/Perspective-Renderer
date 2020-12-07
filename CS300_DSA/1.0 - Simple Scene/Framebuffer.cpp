//
// Created by user on 11/28/2020.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: Framebuffer.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Simple_Scene
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 11/28/2020
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "Framebuffer.h"

GLuint Framebuffer::CreateFrameBuffer(const std::vector<GLuint> &cols, GLuint depth) {
    GLuint fbo = 0;
    glCreateFramebuffers(1, &fbo);

    // Attach each color to FBO
    for (unsigned i = 0; i < cols.size(); i++)
    {
        glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0 + i, cols[i], 0);
    }

    std::array<GLenum, 32> draw_buffs;
    for (GLenum i = 0; i < cols.size(); i++)
    {
        draw_buffs[i] = GL_COLOR_ATTACHMENT0 + i;
    }

    glNamedFramebufferDrawBuffers(fbo, cols.size(), draw_buffs.data());

    if (depth != GL_NONE)
    {
        glNamedFramebufferTexture(fbo, GL_DEPTH_ATTACHMENT, depth, 0);
    }

    if (glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        throw std::runtime_error("incomplete framebuffer");
    }
    return fbo;
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &framebufferID);
}

Framebuffer::Framebuffer(GLuint id) : framebufferID(id){}

void Framebuffer::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
}

void Framebuffer::Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint Framebuffer::GetID() const {
    return framebufferID;
}

void Framebuffer::Clear(GLenum buffer, GLint drawBuffer, const GLfloat  *value) {
    glClearNamedFramebufferfv(framebufferID, buffer, drawBuffer, const_cast<GLfloat*>(value));
}
