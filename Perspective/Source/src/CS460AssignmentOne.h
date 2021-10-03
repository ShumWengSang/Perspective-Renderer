


#pragma once

#include "App.h"

class CS460AssignmentOne final : public App {
public:
    CS460AssignmentOne() = default;

    virtual ~CS460AssignmentOne() = default;

    Settings Setup() override;

    void Init() override;

    void Resize(int width, int height) override;

    void Draw(Input const &input, float deltaTime, float runningTime) override;
};



