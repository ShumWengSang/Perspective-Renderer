//
// Created by user on 11/5/2020.
//

//
// Created by user on 11/5/2020.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: VertexArray.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Simple_Scene
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 11/5/2020
 * End Header --------------------------------------------------------*/


#ifndef SIMPLE_SCENE_VERTEXARRAY_H
#define SIMPLE_SCENE_VERTEXARRAY_H


class VertexArray {
public:
    explicit VertexArray(GLuint id);
    VertexArray(VertexArray const &) = delete;
    VertexArray& operator=(const VertexArray&) = delete;
    ~VertexArray();

    GLuint GetID();
    void SetID(GLuint ID);
    VertexArray& PushBuffer(std::initializer_list<GLuint> buffers);

    void Bind() const;
    void Unbind() const;

    void SetIndexSize(unsigned size);
    unsigned GetIndexSize();
private:
    void DestroyVA();
    unsigned indexSize = 0;
    GLuint ID;
    std::vector<GLuint> Buffers;
};


#endif //SIMPLE_SCENE_VERTEXARRAY_H
