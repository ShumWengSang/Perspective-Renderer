//
// Created by user on 1/27/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: Uniform.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/27/2021
 * End Header --------------------------------------------------------*/
#include "stdafx.h"
#include "Quaternions.h"

MyMath::Quaternion::Quaternion() : s(1), v(0) {}

MyMath::Quaternion::Quaternion(const glm::quat &quat) : s(quat.w), v({quat.x, quat.y, quat.z}) {
}

MyMath::Quaternion::Quaternion(float S, const glm::vec3 &V)
        : s(S), v(V) {}

MyMath::Quaternion::Quaternion(glm::mat4 const &mat) {
    s = 0.5 * sqrt(mat[0][0] + mat[1][1] + mat[2][2] + 1);
    v.x = (mat[2][1] - mat[1][2]) / (4 * s);
    v.y = (mat[0][2] - mat[2][0]) / (4 * s);
    v.z = (mat[1][0] - mat[0][1]) / (4 * s);
}

void MyMath::Quaternion::operator+=(const MyMath::Quaternion &rhs) {
    s += rhs.s;
    v += rhs.v;
}

MyMath::Quaternion MyMath::Quaternion::operator+(const MyMath::Quaternion &rhs) const {
    float scalar = this->s + rhs.s;
    glm::vec3 vector = this->v + rhs.v;
    return Quaternion(scalar, vector);
}

//void MyMath::Quaternion::operator-=(const MyMath::Quaternion &rhs) {
//    s -= rhs.s;
//    v -= rhs.v;
//}
//
//MyMath::Quaternion MyMath::Quaternion::operator-(const MyMath::Quaternion &rhs) const {
//    float scalar = this->s - rhs.s;
//    glm::vec3 vector = this->v - rhs.v;
//    return Quaternion(scalar, vector);
//}

void MyMath::Quaternion::operator*=(const MyMath::Quaternion &q) {
    auto olds = s;
    s = s * q.s - glm::dot(v, q.v);
    v = q.v * olds + v * q.s + glm::cross(v, q.v);
}

MyMath::Quaternion MyMath::Quaternion::operator*(const MyMath::Quaternion &q) const {
    float scalar = s * q.s - glm::dot(v, q.v);

    glm::vec3 imaginary = q.v * s + v * q.s + glm::cross(v, q.v);

    return Quaternion(scalar, imaginary);
}

float MyMath::Quaternion::Dot(const MyMath::Quaternion &q) const {
    return q.s * s + glm::dot(v, q.v);
}

void MyMath::Quaternion::operator*=(const float value) {
    s *= value;
    v *= value;
}

MyMath::Quaternion MyMath::Quaternion::operator*(const float value) const {
    float scalar = s * value;
    glm::vec3 imaginary = v * value;

    return Quaternion(scalar, imaginary);
}

MyMath::Quaternion MyMath::Quaternion::operator/(const float value) const {
    return *this * (1 / value);
}

float MyMath::Quaternion::MagSquared() const {
    float scalar = s * s;
    float imaginary = glm::dot(v, v);
    return scalar + imaginary;
}

float MyMath::Quaternion::Magnitude() const { return sqrt(MagSquared()); }

void MyMath::Quaternion::Normalize() {
    float Mag = Magnitude();
    if (Mag != 0) {
        float normValue = 1 / Mag;
        v *= normValue;
        s *= normValue;
    }
}

MyMath::Quaternion MyMath::Quaternion::Conjugate() const {
    float scalar = s;
    glm::vec3 imaginary = -v;
    return Quaternion(scalar, imaginary);
}

MyMath::Quaternion MyMath::Quaternion::Inverse() const {
    float absoluteValue = MagSquared();
    absoluteValue = std::abs(absoluteValue);
    absoluteValue = 1 / absoluteValue;

    Quaternion conjugateValue = Conjugate();

    float scalar = conjugateValue.s * absoluteValue;
    glm::vec3 imaginary = conjugateValue.v * absoluteValue;

    return Quaternion(scalar, imaginary);
}

void MyMath::Quaternion::ConvertToSpecialForm() {
    float angle = glm::radians(s);
    v = glm::normalize(v);
    s = cosf(angle * 0.5);
    v = v * sinf(angle * 0.5);
}

glm::vec3 MyMath::Quaternion::RotateAngleAxis(float alphaAngle, const glm::vec3 &uAxis, const glm::vec3 &rotVectorR) {
    // convert our vector to a pure quaternion
    Quaternion p(0.f, rotVectorR);

    // normalize the axis
    glm::vec3 normalizedAxis = glm::normalize(uAxis);

    // create the real quaternion
    Quaternion q(alphaAngle, normalizedAxis);

    // convert quaternion to unit norm quaternion
    q.ConvertToSpecialForm();

    // Get the inverse of the quaternion
    Quaternion qInverse = q.Inverse();

    // rotate the quaternion
    Quaternion rotatedVector = q * p * qInverse;

    // return the vector part of the quaternion
    return rotatedVector.v;
}

MyMath::Quaternion MyMath::Quaternion::Norm() const {
    float Mag = Magnitude();
    if (Mag != 0) {
        Quaternion result;
        float normValue = 1 / Mag;
        result.v = v * normValue;
        result.s = s * normValue;
        return result;
    }
    // errr lol
    return *this;
}

glm::vec3 MyMath::Quaternion::Rotate(glm::vec3 point) const {
    Quaternion norm = this->Norm();
    return (norm * Quaternion(0, point) * norm.Inverse()).v;
}

glm::mat4 MyMath::Quaternion::ToMat4(bool normalize) const {
    Quaternion norm = *this;
    if (normalize)
        norm = this->Norm();

    glm::mat4 result(1.0f);

    result[0][0] = 1 - 2 * (norm.v.y * norm.v.y + norm.v.z * norm.v.z);
    result[1][0] = 2 * (norm.v.x * norm.v.y - norm.v.z * norm.s);
    result[2][0] = 2 * (norm.v.x * norm.v.z + norm.v.y * norm.s);

    result[0][1] = 2 * (norm.v.x * norm.v.y + norm.s * norm.v.z);
    result[1][1] = 1 - 2 * (norm.v.x * norm.v.x + norm.v.z * norm.v.z);
    result[2][1] = 2 * (norm.v.y * norm.v.z - norm.s * norm.v.x);

    result[0][2] = 2 * (norm.v.x * norm.v.z - norm.s * norm.v.y);
    result[1][2] = 2 * (norm.v.y * norm.v.z + norm.s * norm.v.x);
    result[2][2] = 1 - 2 * (norm.v.x * norm.v.x + norm.v.y * norm.v.y);

    return result;
}

MyMath::Quaternion MyMath::Quaternion::GetRotationBetween(const glm::vec3 &u, const glm::vec3 &v) {
    float k_cos_theta = glm::dot(u,v);
    float k = sqrt(glm::length2(u) * glm::length2(v));
    if((k_cos_theta / k) == -1)
    {
        // 180 degree rotation around any orthogonal vector
        return Quaternion(0, glm::normalize(orthogonal(u)));
    }
    return Quaternion(k_cos_theta + k, glm::cross(u, v)).Norm();
}

MyMath::VQS::VQS(const glm::vec3 &V, const MyMath::Quaternion &Q, const float S)
        : v(V), q(Q), s(S) {}

MyMath::VQS::VQS(const glm::mat4 &transformation) {
    v = glm::vec3(transformation[3]);
    s = glm::length(transformation[0]);
    q = MyMath::Quaternion(glm::quat_cast(transformation / s));

}

glm::vec3 MyMath::VQS::operator*(const glm::vec3 &p) const {
    return s * q.Rotate(p) + v;
}

MyMath::VQS MyMath::VQS::operator*(const MyMath::VQS &rhs) const {
    glm::vec3 v = s * q.Rotate(rhs.v) + this->v;
    Quaternion Q = q * rhs.q;
    float S = s * rhs.s;
    return VQS(v, Q, S);
}

MyMath::VQS MyMath::VQS::Inverse() const {
    glm::vec3 V = 1 / s * (q.Inverse().Rotate(-v));
    Quaternion Q = q.Inverse();
    float S = 1 / s;
    return VQS(V, Q, S);
}

glm::mat4 MyMath::VQS::ToMat4() const {
    glm::mat4 t = glm::translate(v);
    glm::mat4 scale = glm::scale(glm::vec3(s, s, s));
    // Scale first, then rotate, then translate
    return t * q.ToMat4() * scale;
}

void MyMath::VQS::operator*=(const MyMath::VQS &rhs) {
    v = s * q.Rotate(rhs.v) + v;
    q = q * rhs.q;
    s = s * rhs.s;
}

glm::vec3 MyMath::orthogonal(const glm::vec3 &v) {
    float x = abs(v.x);
    float y = abs(v.y);
    float z = abs(v.z);

    glm::vec3 X_AXIS (1,0,0);
    glm::vec3 Y_AXIS (0,1,0);
    glm::vec3 Z_AXIS (0,0,1);

    glm::vec3 other = x < y ? (x < z ? X_AXIS : Z_AXIS) : (y < z ? Y_AXIS : Z_AXIS);
    return glm::cross(v, other);
}
