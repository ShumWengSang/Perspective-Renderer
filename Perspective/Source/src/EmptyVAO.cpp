//
// Created by user on 1/27/2021.
//

#include "stdafx.h"
#include "EmptyVAO.h"

void EmptyVAO::Draw() {
    static GLuint emptyVertex = 0;
    if(!emptyVertex)
    {
        glCreateVertexArrays(1, &emptyVertex);
    }
    GLint lastBoundVertexArray;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &lastBoundVertexArray);

    glBindVertexArray(emptyVertex);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(lastBoundVertexArray);
}