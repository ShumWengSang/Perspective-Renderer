//
// Created by roland on 9/24/20.
//
#include "../stdafx.h"
#include "OpenGLBuffer.h"

static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
    switch(type)
    {
        case ShaderDataType::Float: return GL_FLOAT;
        case ShaderDataType::Float2: return GL_FLOAT;
        case ShaderDataType::Float3: return GL_FLOAT;
        case ShaderDataType::Float4: return GL_FLOAT;

        case ShaderDataType::Mat3: return GL_FLOAT;
        case ShaderDataType::Mat4: return GL_FLOAT;

        case ShaderDataType::Int: return GL_INT;
        case ShaderDataType::Int2: return GL_INT;
        case ShaderDataType::Int3: return GL_INT;
        case ShaderDataType::Int4: return GL_INT;

        case ShaderDataType::Bool: return GL_BOOL;
        default: DEBUG_BREAKPOINT();
    }
    return 0;
}

OpenGLVertexBuffer::OpenGLVertexBuffer(size_t size) {
    glCreateBuffers(1, &RenderID);
    glBindBuffer(GL_ARRAY_BUFFER, RenderID);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

OpenGLVertexBuffer::OpenGLVertexBuffer(float *vertices, size_t size) {
    glCreateBuffers(1, &RenderID);
    glBindBuffer(GL_ARRAY_BUFFER, RenderID);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() {
    glDeleteBuffers(1, &RenderID);
}

void OpenGLVertexBuffer::Bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, RenderID);
}

void OpenGLVertexBuffer::Unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLVertexBuffer::SetData(const void *data, unsigned int size) {
    this->Bind();
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

void OpenGLVertexBuffer::SetLayout(const BufferLayout &layout) {
    this->Layout = layout;
}

const BufferLayout &OpenGLVertexBuffer::GetLayout() {
    return Layout;
}



void OpenGLIndexBuffer::Bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RenderID);
}

void OpenGLIndexBuffer::Unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

size_t OpenGLIndexBuffer::GetCount() const {
    return Count;
}

OpenGLIndexBuffer::OpenGLIndexBuffer(unsigned int *indices, size_t count) : Count(count){
    glCreateBuffers(1, &RenderID);
    glBindBuffer(GL_ARRAY_BUFFER, RenderID);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(unsigned), indices, GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer() {
    glDeleteBuffers(1, &RenderID);
}

void OpenGLVertexArray::SetIndexBuffer(const SharedPtr<IndexBuffer> &indexBuffer) {
    glBindVertexArray(RendererID);
    indexBuffer->Bind();
    internal_IndexBuffer = indexBuffer;
}

const std::vector<SharedPtr<VertexBuffer>> &OpenGLVertexArray::GetVertexBuffers() const {
    return VertexBuffers;
}

const SharedPtr<IndexBuffer> &OpenGLVertexArray::GetIndexBuffer() const {
    return internal_IndexBuffer;
}

void OpenGLVertexArray::AddVertexBuffer(const SharedPtr<VertexBuffer> &vertexBuffer) {
    if(vertexBuffer->GetLayout().GetElements().empty())
    {
        // NEED VERTEX BUFFER LAYOUT
        DEBUG_BREAKPOINT();
    }

    glBindVertexArray(RendererID);
    vertexBuffer->Bind();

    const auto& layout = vertexBuffer->GetLayout();
    for(const auto& element : layout)
    {
        switch(element.Type)
        {
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
            case ShaderDataType::Bool:
            {
                glEnableVertexAttribArray(VertexBufferIndex);
                glVertexAttribPointer(VertexBufferIndex,
                                      element.GetComponentCount(),
                                      ShaderDataTypeToOpenGLBaseType(element.Type),
                                      element.Normalized ? GL_TRUE : GL_FALSE,
                                      layout.GetStride(),
                                      (const void*)(element.Offset));
                VertexBufferIndex++;
                break;
            }
            case ShaderDataType::Mat3:
            case ShaderDataType::Mat4: {
                size_t count = element.GetComponentCount();
                for (unsigned i = 0; i < count; ++i) {
                    glEnableVertexAttribArray(VertexBufferIndex);
                    glVertexAttribPointer(VertexBufferIndex,
                                          count,
                                          ShaderDataTypeToOpenGLBaseType(element.Type),
                                          element.Normalized ? GL_TRUE : GL_FALSE,
                                          layout.GetStride(),
                                          (const void *) (element.Offset + sizeof(float) * count * i));
                    glVertexAttribDivisor(VertexBufferIndex, 1);
                    VertexBufferIndex++;
                }
                break;
            }

            default: DEBUG_BREAKPOINT();
        }
    }
    VertexBuffers.emplace_back(vertexBuffer);
}

void OpenGLVertexArray::Unbind() const {
    glBindVertexArray(0);
}

void OpenGLVertexArray::Bind() const {
    glBindVertexArray(RendererID);
}

OpenGLVertexArray::~OpenGLVertexArray() {
    glDeleteVertexArrays(1, &RendererID);
}
