//
// Created by roland on 9/24/20.
//
/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: OpenGLBuffer.h
 * Purpose: Child class of abstracted base class Buffer, OpenGL specific.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: RolandShum_CS300_1
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 3rd October 2020
 * End Header --------------------------------------------------------*/

#ifndef SIMPLE_SCENE_OPENGLBUFFER_H
#define SIMPLE_SCENE_OPENGLBUFFER_H

#include "../Renderer/Buffer.h"
class OpenGLVertexBuffer : public VertexBuffer {
public:
    OpenGLVertexBuffer(size_t size);
    OpenGLVertexBuffer(float * vertices, size_t size);
    ~OpenGLVertexBuffer() override;

    void Bind() const override;

    void Unbind() const override;

    void SetData(const void *data, unsigned int size) override;

    void SetLayout(const BufferLayout &layout) override;

    const BufferLayout &GetLayout() override;

private:
    unsigned RenderID;
    BufferLayout Layout;

};

class OpenGLIndexBuffer : public IndexBuffer
{
public:
    OpenGLIndexBuffer(GLuint * indices, size_t count);
    ~OpenGLIndexBuffer() override;

    void Bind() const override;

    void Unbind() const override;

    size_t GetCount() const override;

private:
    unsigned RenderID;
    unsigned Count;
};

class OpenGLVertexArray : public VertexArray
{
public:
    OpenGLVertexArray(VertexArray::PrimitiveType type);
    ~OpenGLVertexArray() override;

    const PrimitiveType GetType() const override;

    virtual void Bind() const override;
    virtual void Unbind() const override;

    virtual void AddVertexBuffer(const SharedPtr<VertexBuffer>& vertexBuffer) override;

    void SetIndexBuffer(const SharedPtr<IndexBuffer> &indexBuffer) override;

    const std::vector<SharedPtr<VertexBuffer>> &GetVertexBuffers() const override;

    const SharedPtr<IndexBuffer> &GetIndexBuffer() const override;

private:
    unsigned RendererID;
    unsigned VertexBufferIndex = 0;
    std::vector<SharedPtr<VertexBuffer>> VertexBuffers;
    SharedPtr<IndexBuffer> internal_IndexBuffer;
    VertexArray::PrimitiveType primitiveType;
};

#endif //SIMPLE_SCENE_OPENGLBUFFER_H
