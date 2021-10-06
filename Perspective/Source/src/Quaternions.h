//
// Created by user on 1/27/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: Uniform.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/27/2021
 * End Header --------------------------------------------------------*/

#ifndef OPENGLFRAMEWORK_QUATERNION_H
#define OPENGLFRAMEWORK_QUATERNION_H

namespace MyMath {
	class Quaternion {
	public:
		Quaternion();
		Quaternion(float S, const glm::vec3& V);
		Quaternion(const Quaternion& value) = default;
		Quaternion& operator=(const Quaternion& value) = default;
		~Quaternion() = default;
		Quaternion(glm::mat4 const& mat);
		// Adding and subtraction
		void operator+=(const Quaternion& rhs);
		Quaternion operator+(const Quaternion& rhs) const;
		//void operator-=(const Quaternion& rhs);
		//Quaternion operator-(const Quaternion& rhs) const;

		// Product
		void operator*=(const Quaternion& q);
		Quaternion operator*(const Quaternion& q) const;
		float Dot(const Quaternion& q) const;

		// Scalar multiplication
		void operator*=(const float value);
		Quaternion operator*(const float value) const;

		// Magnitude
		float MagSquared() const;
		float Magnitude() const;

		// Normalize
		void Normalize();
		Quaternion Norm() const;

		// Conjugate
		Quaternion Conjugate() const;
		// Inverse
		Quaternion Inverse() const;

		void ConvertToSpecialForm();

		glm::vec3 Rotate(glm::vec3 point) const;

		float GetAngle() const { return 2 * acosf(s); }

		// Rotation
		static glm::vec3 RotateAngleAxis(float alphaAngle, const glm::vec3& uAxis,
			const glm::vec3& rotVectorR);

		// Matrix back and forth
		glm::mat4 ToMat4(bool normalize = true) const;

		// To glm Quats
		glm::quat ToGLMQuat() const
		{
			return glm::quat(s, v.x, v.y, v.z);
		}
		
	public:
		float s{};
		glm::vec3 v{};
	};

	class VQS {
	public:
		VQS() = default;
		VQS(const glm::vec3& V, const Quaternion& Q, const float S);

	public:
		// Rotate
		glm::vec3 operator*(const glm::vec3& p) const;

		// Composition of VQS
		VQS operator*(const VQS& rhs) const;
		void operator*=(const VQS& rhs);

		// Inverse
		VQS Inverse() const;

		// To Mat4
		glm::mat4 ToMat4() const;

	public:
		glm::vec3 v{};
		Quaternion q{};
		float s{};
	};
}  // namespace MyMath
#endif  // OPENGLFRAMEWORK_UNIFORM_H
