//
// Created by roland on 9/24/20.
//
#include "../stdafx.h"
#include "Buffer.h"
#include "../OpenGL/OpenGLBuffer.h"

void BufferLayout::CalcOffsetAndStride() {
    size_t offset = 0;
    Stride = 0;
    for (auto &element : Elements) {
        element.Offset = offset;
        offset += element.Size;
        Stride += element.Size;
    }
}

BufferLayout::BufferLayout(const std::initializer_list<BufferElement> &elements) : Elements(
        elements) { CalcOffsetAndStride(); }

const std::vector<BufferElement> &BufferLayout::GetElements() const {
    return Elements;
}

unsigned BufferLayout::GetStride() const { return Stride; }

BufferLayout::Iterator BufferLayout::begin() { return Elements.begin(); }

BufferLayout::Iterator BufferLayout::end() { return Elements.end(); }

BufferLayout::ConstIterator BufferLayout::begin() const { return Elements.cbegin(); }

BufferLayout::ConstIterator BufferLayout::end() const { return Elements.cend(); }

unsigned BufferElement::GetComponentCount() const {
    switch (Type) {
        case ShaderDataType::Float:
            return 1;
        case ShaderDataType::Float2:
            return 2;
        case ShaderDataType::Float3:
            return 3;
        case ShaderDataType::Float4:
            return 4;

        case ShaderDataType::Mat3:
            return 3;
        case ShaderDataType::Mat4:
            return 4;

        case ShaderDataType::Int:
            return 1;
        case ShaderDataType::Int2:
            return 2;
        case ShaderDataType::Int3:
            return 3;
        case ShaderDataType::Int4:
            return 4;

        case ShaderDataType::Bool:
            return 1;
        default:
            DEBUG_BREAKPOINT();
    }
    return 0;
}

BufferElement::BufferElement(ShaderDataType type, const std::string &name, bool normalized) :
        Name(name),
        Type(type),
        Size(ShaderDataTypeSize(type)),
        Offset(0),
        Normalized(normalized) {}

SharedPtr<VertexBuffer> VertexBuffer::Create(size_t size) {

    // Depending on API, we can change this around
    return CreateSharedPtr<OpenGLVertexBuffer>(size);
    return nullptr;
}

SharedPtr<VertexBuffer> VertexBuffer::Create(float *vertices, size_t size) {
    // Depending on API, we can change this around
    return CreateSharedPtr<OpenGLVertexBuffer>(vertices, size);
    return nullptr;
}

SharedPtr<IndexBuffer> IndexBuffer::Create(int *vertices, size_t size) {
    // Depending on API, we can change this around
    return CreateSharedPtr<OpenGLIndexBuffer>(vertices, size);
    return nullptr;
}

SharedPtr<VertexArray> VertexArray::Create() {
    return CreateSharedPtr<OpenGLVertexArray>();
    return nullptr;
}
