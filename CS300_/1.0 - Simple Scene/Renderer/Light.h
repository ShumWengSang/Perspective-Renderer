//
// Created by roland on 10/22/20.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: Light
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: RolandShum_CS300_1
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 3rd October 2020
 * End Header --------------------------------------------------------*/
#ifndef SIMPLE_SCENE_LIGHT_H
#define SIMPLE_SCENE_LIGHT_H


class Light {
public:
    enum class LightType : unsigned
    {
        DirectionalLight,
        SpotLight,
        PointLight,
    };

    struct Global
    {
        unsigned numOfLights;
        unsigned c_1, c_2, c_3;

    };

    struct LightUniform
    {
        glm::vec3 lightPosition;
        glm::vec3 lightDirection;
        glm::vec3 lightColor;

        // Spotlight
        float theta, phi;
    };
};


#endif //SIMPLE_SCENE_LIGHT_H
