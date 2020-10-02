//
// Created by roland on 9/24/20.
//

#ifndef SIMPLE_SCENE_BUFFER_H
#define SIMPLE_SCENE_BUFFER_H

// Shader data type
enum class ShaderDataType
{
    None = 0,
    Float,
    Float2,
    Float3,
    Float4,
    Mat3,
    Mat4,
    Int,
    Int2,
    Int3,
    Int4,
    Bool
};

unsigned ShaderDataTypeSize(ShaderDataType type);

class BufferElement {
public:
    std::string Name;
    ShaderDataType Type;
    unsigned Size;
    size_t Offset;
    bool Normalized;

    BufferElement() = default;
    BufferElement(ShaderDataType type,
                  std::string const & name,
                  bool normalized = false);

    unsigned GetComponentCount() const;

};

class BufferLayout
{
    using Iterator = std::vector<BufferElement>::iterator;
    using ConstIterator = std::vector<BufferElement>::const_iterator;
public:
    BufferLayout() = default;
    BufferLayout(std::initializer_list<BufferElement> const & elements);

    unsigned GetStride() const;

    const std::vector<BufferElement>& GetElements() const;

    Iterator begin();
    Iterator end();
    ConstIterator begin() const;
    ConstIterator end() const;

private:
    std::vector<BufferElement> Elements;
    unsigned Stride = 0;

    void CalcOffsetAndStride();
};

class VertexBuffer
{
public:

    virtual ~VertexBuffer() = default;
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    virtual void SetData(void const * data, unsigned size) = 0;

    virtual void SetLayout(BufferLayout const &) = 0;
    virtual const BufferLayout& GetLayout() = 0;

    static SharedPtr<VertexBuffer> Create(size_t size);
    static SharedPtr<VertexBuffer> Create(float * vertices, size_t size);
};

class IndexBuffer
{
public:
    virtual ~IndexBuffer() = default;
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual size_t GetCount() const =0;

    static SharedPtr<IndexBuffer> Create(unsigned * vertices, size_t count);
};

class VertexArray
{
public:
    enum class PrimitiveType
    {
        Triangles,
        Lines,
        None
    };

    virtual ~VertexArray() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual void AddVertexBuffer(SharedPtr<VertexBuffer> const & vertexBuffer) = 0;
    virtual void SetIndexBuffer(SharedPtr<IndexBuffer> const & indexBuffer) = 0;

    virtual const std::vector<SharedPtr<VertexBuffer>> & GetVertexBuffers() const = 0;
    virtual const SharedPtr<IndexBuffer> & GetIndexBuffer() const = 0;

    virtual const PrimitiveType GetType() const = 0;

    static SharedPtr<VertexArray> Create(PrimitiveType type = PrimitiveType::Triangles);
};

#endif //SIMPLE_SCENE_BUFFER_H
