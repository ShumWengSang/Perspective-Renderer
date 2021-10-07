#include "stdafx.h"
#include "Mesh.h"
#include <ShaderLocations.h>

Mesh::Mesh(
        std::vector<Vertex> const &vertices, std::vector<unsigned int> const &indices
        , std::vector<Texture> const &textures
          ) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

void Mesh::Draw() const {
    if (VAO) {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
    }
}

void Mesh::setupMesh() {
    GLuint indexBuffer;
    GLsizei indexCount;
    GLenum &indexType = EBO;
    {
        glCreateBuffers(1, &indexBuffer);

        const auto &data = this->indices;

        indexCount = static_cast<GLsizei>(data.size());
        indexType = GL_UNSIGNED_INT;

        size_t size = sizeof(uint32_t) * indexCount;

        GLbitfield flags = GL_DYNAMIC_STORAGE_BIT; // TODO: Consider these! Good default?
        glNamedBufferStorage(indexBuffer, size, data.data(), flags);
    }

    GLuint &vertexBuffer = VBO;
    {
        glCreateBuffers(1, &vertexBuffer);

        const auto &data = this->vertices;
        size_t size = sizeof(Vertex) * data.size();
        GLbitfield flags = GL_DYNAMIC_STORAGE_BIT; // TODO: Consider these! Good default?
        glNamedBufferStorage(vertexBuffer, size, data.data(), flags);
    }

    GLuint &vao = VAO;
    glCreateVertexArrays(1, &vao);

    // Specify the element buffer for this vertex array
    glVertexArrayElementBuffer(vao, indexBuffer);

    // Bind the vertex array to a specific binding index and specify it stride, etc.
    GLuint vertexArrayBindingIndex = 0;
    glVertexArrayVertexBuffer(vao, vertexArrayBindingIndex, vertexBuffer, 0, sizeof(Vertex));

    // Enable the attribute, specify its format, and connect the vertex array (at its
    // binding index) to to this specific attribute for this vertex array
    glEnableVertexArrayAttrib(vao, PredefinedAttributeLocation(a_position));
    glVertexArrayAttribFormat(vao, PredefinedAttributeLocation(a_position), 3, GL_FLOAT, GL_FALSE,
                              offsetof(Vertex, position));
    glVertexArrayAttribBinding(vao, PredefinedAttributeLocation(a_position), vertexArrayBindingIndex);

    glEnableVertexArrayAttrib(vao, PredefinedAttributeLocation(a_normal));
    glVertexArrayAttribFormat(vao, PredefinedAttributeLocation(a_normal), 3, GL_FLOAT, GL_FALSE,
                              offsetof(Vertex, normal));
    glVertexArrayAttribBinding(vao, PredefinedAttributeLocation(a_normal), vertexArrayBindingIndex);

    glEnableVertexArrayAttrib(vao, PredefinedAttributeLocation(a_tex_coord));
    glVertexArrayAttribFormat(vao, PredefinedAttributeLocation(a_tex_coord), 2, GL_FLOAT, GL_FALSE,
                              offsetof(Vertex, texCoord));
    glVertexArrayAttribBinding(vao, PredefinedAttributeLocation(a_tex_coord), vertexArrayBindingIndex);

    glEnableVertexArrayAttrib(vao, PredefinedAttributeLocation(a_tangent));
    glVertexArrayAttribFormat(vao, PredefinedAttributeLocation(a_tangent), 4, GL_FLOAT, GL_FALSE,
                              offsetof(Vertex, tangent));
    glVertexArrayAttribBinding(vao, PredefinedAttributeLocation(a_tangent), vertexArrayBindingIndex);

    glEnableVertexArrayAttrib(vao, PredefinedAttributeLocation(a_boneIDs));
    glVertexArrayAttribIFormat(vao, PredefinedAttributeLocation(a_boneIDs), 4, GL_INT, offsetof(Vertex, boneIDs));
    glVertexArrayAttribBinding(vao, PredefinedAttributeLocation(a_boneIDs), vertexArrayBindingIndex);

    glEnableVertexArrayAttrib(vao, PredefinedAttributeLocation(a_weights));
    glVertexArrayAttribFormat(vao, PredefinedAttributeLocation(a_weights), 4, GL_FLOAT, GL_FALSE,
                              offsetof(Vertex, weights));
    glVertexArrayAttribBinding(vao, PredefinedAttributeLocation(a_weights), vertexArrayBindingIndex);
}
