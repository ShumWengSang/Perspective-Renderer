//
// Created by user on 1/27/2021.
//

#ifndef OPENGLFRAMEWORK_EMPTYVAO_H
#define OPENGLFRAMEWORK_EMPTYVAO_H
namespace EmptyVAO
{
    void Draw()
    {
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
}
#endif //OPENGLFRAMEWORK_EMPTYVAO_H
