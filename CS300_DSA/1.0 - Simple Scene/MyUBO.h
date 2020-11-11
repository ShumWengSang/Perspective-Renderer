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
 * File Name: MyUBO.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Simple_Scene
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 11/6/2020
 * End Header --------------------------------------------------------*/


#ifndef SIMPLE_SCENE_MYUBO_H
#define SIMPLE_SCENE_MYUBO_H

class MyUBO {
public:
    glm::mat4 Projection = glm::mat4(1.0f);
    glm::mat4 View = glm::mat4(1.0f);
    glm::vec4 NearFar = {}; // x = near, y = far
    glm::ivec4 modes = {}; // x = GPU mode, y = toggle for normal
};

class LightUBO{
public:
    glm::vec4 lightPos[16] = {};
    glm::vec4 lightDiffuseColor [16] = {{1, 1, 1, 1}};
    glm::vec4 lightAmbientColor [16] = {{0.113f, 0.113f, 0.113f, 1}};
    glm::vec4 lightSpecularColor [16] = {{1, 1, 1, 1}};
    glm::vec4 lightInfo[16] = {{0.0f, 8.0f, 0.151, 0.434}}; // x = light type, y = spotlight fall off , z = theta, w = phi
    glm::vec4 lightDir[16] = {};
    glm::vec4 cameraPosition = {};
    glm::vec4 fogColor = glm::vec4(0.0f);
    glm::vec4 globalAmbientColor = glm::vec4(0.0f);
    glm::vec4 coEfficients  = glm::vec4(0.1, 1.0, 1.0, 1.0); // r = ambient, b = diffuse, c = specular
    struct
    {
        float numOfLight = 1;
        float c1 = 0.996f, c2 = 0.373f, c3 = 0.217f;
    };
};

#endif //SIMPLE_SCENE_MYUBO_H
