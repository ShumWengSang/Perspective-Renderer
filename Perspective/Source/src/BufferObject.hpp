//
// Created by user on 1/26/2021.
//

#ifndef OPENGLFRAMEWORK_BUFFEROBJECT_H
#define OPENGLFRAMEWORK_BUFFEROBJECT_H
enum class BufferObjectType
{
    Uniform = GL_UNIFORM_BUFFER
};

template <typename T>
class BufferObject
{
public:
    BufferObject() = default;
    BufferObject(const BufferObject& other) = delete;
    BufferObject& operator=(const BufferObject& other) = delete;

    void BindBufferBase(BufferObjectType type, GLuint index);
    void UpdateGpuBuffer();

    T memory{};

private:
    void CreateBufferIfRequired();
    T gpuMemory{};
    GLuint bufferHandle;
};

template <typename T>
void BufferObject<T>::CreateBufferIfRequired() {
    if(!bufferHandle)
    {
        glCreateBuffers(1, &bufferHandle);
        glNamedBufferStorage(bufferHandle, sizeof(T), nullptr, GL_DYNAMIC_STORAGE_BIT); // Maybe change?
    }
}

template <typename T>
void BufferObject<T>::BindBufferBase(BufferObjectType type, GLuint index) {
    CreateBufferIfRequired();
    GLenum target = static_cast<GLenum>(type);
    glBindBufferBase(target, index, bufferHandle);
}

template <typename T>
void BufferObject<T>::UpdateGpuBuffer() {
    CreateBufferIfRequired();
    if (memcmp(&memory, &gpuMemory, sizeof(T)) != 0)
    {
        gpuMemory = memory;
        glNamedBufferSubData(bufferHandle, 0, sizeof(T), &gpuMemory);
    }
}
#endif //OPENGLFRAMEWORK_BUFFEROBJECT_H
