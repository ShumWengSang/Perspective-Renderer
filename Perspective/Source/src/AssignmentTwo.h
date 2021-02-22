//
// Created by user on 2/20/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: AssignmentTwo.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Perspective
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 2/20/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_ASSIGNMENTTWO_H
#define OPENGLFRAMEWORK_ASSIGNMENTTWO_H
#include "App.h"
class TransformSystem;
class Material;

namespace Shapes {
    class Shape;
}
using Shapes::Shape;

class AssignmentTwo : public App {
public:
    AssignmentTwo();
    virtual ~AssignmentTwo() = default;

    Settings Setup() override;

    void Init() override;
    void Resize(int width, int height) override;
    void Draw(Input const & input, float deltaTime, float runningTime) override;
    void Update(const Input &input, float dt);
    TransformSystem& transformSystem;

private:
    void LoadModelFromTextFile(std::string fileName, Material* = nullptr);
    float powerPlantScale = 1.0f / 16000;
    float imGuiPowerPlantScale = 1.0f;

    void AddShape(std::shared_ptr<Shape> shape);
    std::vector<std::shared_ptr<Shape>> Shapes;

    static void RunTestCases();
};


#endif //OPENGLFRAMEWORK_ASSIGNMENTTWO_H
