//
// Created by roland on 9/21/20.
//
#include "stdafx.h"
#include "VertexDataManager.h"

VertexDataManager::VertexDataManager() {

}

VertexDataManager::~VertexDataManager() {
    for(int i = numVBO - 1; i >= 0; ++i)
    {
        glDeleteBuffers(1, &vboID[numVBO]);
    }
    glDeleteVertexArrays(1, &vaoID);
}

VertexDataManager &VertexDataManager::CreateVAO() {
    glGenVertexArrays(1, &vaoID);
    return *this;
}

VertexDataManager &VertexDataManager::CreateVBO(GLuint& id) {
    glGenBuffers(1, &vboID[numVBO]);
    id = numVBO++;
    return *this;
}

VertexDataManager &VertexDataManager::CreateIndexBuffer() {
    glGenBuffers(1, &vboID[numVBO]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID[numVBO]);
    ++numVBO;
    return *this;
}

VertexDataManager& VertexDataManager::BindVertexArrayObject() {
    glBindVertexArray(vaoID);
    return *this;
}

VertexDataManager& VertexDataManager::BindVertexArray(int id, GLuint numBytes, GLfloat *vertexBuffer, GLenum gl_draw_mode) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID[id]);
    glBufferData(GL_ARRAY_BUFFER,
                 numBytes,
                 vertexBuffer,
                 gl_draw_mode);

    return *this;
}

VertexDataManager& VertexDataManager::BindIndexBuffer(GLint numBytes, GLuint *indexBuffer, GLenum gl_draw_mode) {
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 numBytes,
                 indexBuffer,
                 gl_draw_mode);
    return *this;
}
