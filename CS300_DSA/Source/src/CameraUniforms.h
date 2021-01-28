//
// Created by user on 1/26/2021.
//

#ifndef OPENGLFRAMEWORK_CAMERAUNIFORMS_H
#define OPENGLFRAMEWORK_CAMERAUNIFORMS_H
class CameraUniforms
{
public:
    glm::mat4 ViewFromWorldMatrix;
    glm::mat4 ProjectFromWorldMatrix;
    glm::vec4 nearFar;
};
#endif //OPENGLFRAMEWORK_CAMERAUNIFORMS_H
