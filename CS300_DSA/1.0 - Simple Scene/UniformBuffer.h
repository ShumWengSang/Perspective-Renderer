//
// Created by user on 11/6/2020.
//

//
// Created by user on 11/6/2020.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: UniformBuffer.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Simple_Scene
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 11/6/2020
 * End Header --------------------------------------------------------*/


#ifndef SIMPLE_SCENE_UNIFORMBUFFER_H
#define SIMPLE_SCENE_UNIFORMBUFFER_H

template <typename T>
class UniformBuffer {
public:
    inline UniformBuffer(GLuint id, unsigned blockBinding);
    inline T& GetUniformData() ;
    inline void SendData() const;
    inline void SendSubData(GLintptr offset) const;
private:
    GLuint bufferID;
    T uniformData;
};

template<typename T>
UniformBuffer<T>::UniformBuffer(GLuint id, unsigned int blockBinding) : bufferID(id){
    glBindBufferBase(GL_UNIFORM_BUFFER, blockBinding, bufferID);
}

template<typename T>
T &UniformBuffer<T>::GetUniformData()  {
    return uniformData;
}

template<typename T>
void UniformBuffer<T>::SendData() const {
    glNamedBufferSubData(bufferID, 0, sizeof(T), &uniformData);
}

template<typename T>
void UniformBuffer<T>::SendSubData(GLintptr offset) const {
    glNamedBufferSubData(bufferID, offset, sizeof(T) - offset, (char*)(&uniformData) + offset);
}



#endif //SIMPLE_SCENE_UNIFORMBUFFER_H
