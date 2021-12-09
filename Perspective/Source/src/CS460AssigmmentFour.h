


#pragma once

#include "App.h"


class CS460AssignmentFour final : public App {
public:
    CS460AssignmentFour() = default;

    virtual ~CS460AssignmentFour();

    Settings Setup() override;

    void Init() override;

    void Resize(int width, int height) override;

    void Draw(Input const &input, float deltaTime, float runningTime) override;

};



