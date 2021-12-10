


#pragma once

#include "App.h"


class Material;
class Model;
struct Entity;
class CS460AssignmentFour final : public App {
public:
    CS460AssignmentFour() = default;

    virtual ~CS460AssignmentFour();

    Settings Setup() override;

    void Init() override;

    void Resize(int width, int height) override;

    void Draw(Input const &input, float deltaTime, float runningTime) override;

    void Reset();

    Entity& InitEntities(Model* model, float mass,glm::vec3 pos, glm::vec3 scale);

    int mode = 0;

    Material* powerPlantMaterial = nullptr;
    Model* cubeModel = nullptr;
};



