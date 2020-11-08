

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: Vertex.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Simple_Scene
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 11/4/2020
 * End Header --------------------------------------------------------*/


#ifndef SIMPLE_SCENE_VERTEX_H
#define SIMPLE_SCENE_VERTEX_H

#include "Vertex.h"


class Vertex {
public:
    glm::vec3 position, color, normal;
    Vertex(glm::vec3 const& position, glm::vec3 const& color, glm::vec3 const& normal)
    : position(position), color(color), normal(normal){}
};

// This class describes the formatting layout of that one element
class attribFormat
{
public:
    GLuint attrib_index;
    GLint size;
    GLenum type;
    GLuint relative_offset;
};

// Takes a class and returns the number of it and the GL type
// Throws runtime error if unsupported
template<typename T>
constexpr std::pair<GLint, GLenum> typeToSizeEnum()
{
    if constexpr (std::is_same_v<T, float>)
        return std::make_pair(1, GL_FLOAT);
    if constexpr (std::is_same_v<T, int>)
        return std::make_pair(1, GL_INT);
    if constexpr (std::is_same_v<T, unsigned int>)
        return std::make_pair(1, GL_UNSIGNED_INT);
    if constexpr (std::is_same_v<T, glm::vec2>)
        return std::make_pair(2, GL_FLOAT);
    if constexpr (std::is_same_v<T, glm::vec3>)
        return std::make_pair(3, GL_FLOAT);
    if constexpr (std::is_same_v<T, glm::vec4>)
        return std::make_pair(4, GL_FLOAT);
    throw std::runtime_error("unsupported type");
}

// Given an index and an offset, create the attribute format
template<typename T>
inline attribFormat createAttribFormat(GLuint attribIndex, GLuint relativeOffset)
{
    auto const[comp_count, type] = typeToSizeEnum<T>();
    return attribFormat{attribIndex, comp_count, type, relativeOffset };
}

// Create Named Buffer
template<typename T>
inline GLuint createBuffer(std::vector<T> const& buff, GLenum flags = GL_DYNAMIC_STORAGE_BIT)
{
    GLuint name = 0;
    glCreateBuffers(1, &name);
    glNamedBufferStorage(name, sizeof(typename std::vector<T>::value_type) * buff.size(), buff.data(), flags);
    return name;
}

// Return VAO, VBO, IBO
template<typename T>
std::tuple<GLuint, GLuint, GLuint> createGeometry(std::vector<T> const& vertices, const std::vector<uint32_t> &indices, std::vector<attribFormat> const& attrib_formats)
{
    GLuint vao = 0;
    auto vbo = createBuffer(vertices);
    auto ibo = createBuffer(indices);

    glCreateVertexArrays(1, &vao);
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(T));
    glVertexArrayElementBuffer(vao, ibo);

    for (auto const& format : attrib_formats)
    {
        glEnableVertexArrayAttrib(vao, format.attrib_index);
        glVertexArrayAttribFormat(vao, format.attrib_index, format.size, format.type, GL_FALSE, format.relative_offset);
        glVertexArrayAttribBinding(vao, format.attrib_index, 0);
    }

    return std::make_tuple(vao, vbo, ibo);
}

#endif //SIMPLE_SCENE_VERTEX_H
