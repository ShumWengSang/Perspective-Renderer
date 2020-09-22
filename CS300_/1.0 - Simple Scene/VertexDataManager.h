//
// Created by roland on 9/21/20.
//

#ifndef SIMPLE_SCENE_VERTEXDATAMANAGER_H
#define SIMPLE_SCENE_VERTEXDATAMANAGER_H


class VertexDataManager {
public:
    VertexDataManager();
    virtual ~VertexDataManager();

    VertexDataManager& CreateVAO();
    VertexDataManager& CreateVBO(GLuint& id);
    VertexDataManager& CreateIndexBuffer();

    VertexDataManager& BindVertexArrayObject();
    VertexDataManager& BindVertexArray(int id,
                         GLuint numBytes,
                         GLfloat *vertexBuffer,
                         GLenum gl_draw_mode);

    VertexDataManager& BindIndexBuffer(GLint numBytes,
                         GLuint *indexBuffer,
                         GLenum gl_draw_mode);

    VertexDataManager& DescribeDataLayout( 	GLuint layout,
                                              GLint size,
                                              GLenum type,
                                              GLboolean normalized,
                                              GLsizei stride,
                                              const void * pointer);

    VertexDataManager&

    GLuint vaoID = 0;
    GLuint eboID = 0;
    GLuint vboID[8] = {}; // Suggested max of 8 for now

    int numVBO = 0;
};


#endif //SIMPLE_SCENE_VERTEXDATAMANAGER_H
