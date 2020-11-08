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
    glm::vec4 NearFar;
};

class LightUBO{
public:
    glm::vec4 lightPos[16] = {};
    glm::vec4 lightColor[16] = {};
    glm::vec4 lightInfo[16] = {}; // x = light type, y = spotlight fall off , z = theta, w = phi
    // glm::vec4 globalValues; // Num of Light, c1, c2, c3
    glm::vec4 cameraPosition = {};
    glm::vec4 fogColor = glm::vec4(1.0f);
    glm::vec4 globalAmbientColor = glm::vec4(1.0f);
    glm::vec4 coEfficients  = glm::vec4(0.5, 0.3, 0.6, 1.0); // r = ambient, b = diffuse, c = specular
    struct
    {
        float numOfLight;
        float c1 = 0.239f, c2 = 0.115f, c3 = 0.358f;
    };
};

#endif //SIMPLE_SCENE_MYUBO_H
