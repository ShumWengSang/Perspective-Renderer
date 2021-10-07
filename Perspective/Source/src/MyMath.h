//
// Created by user on 1/30/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: Math.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: OpenGLFramework
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 1/30/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_MYMATH_H
#define OPENGLFRAMEWORK_MYMATH_H
#include "Quaternions.h"
class Model;

struct Vertex;
namespace Shapes {
	class Triangle;
}
namespace MyMath {

	float Remap(float value, float low1, float high1, float low2, float high2);

	glm::vec4 Remap(const glm::vec4& value, const glm::vec2& inMinMax, const glm::vec2& outMinMax);

	void CsoSupport(
		const Model& modelA, const Model& modelB, const glm::vec3& dir, glm::vec3& support, glm::vec3& supportA
		, glm::vec3& supportB
	);

	glm::vec3 FindSupportPoint(std::vector<Shapes::Triangle> const& trigs, glm::vec3 const& dir);

	glm::vec3 FindSupportPoint(std::vector<Vertex> const& vertices, glm::vec3 const& dir);

	inline glm::mat4 AssimpToMat4(const aiMatrix4x4& assimpMat)
	{
		return glm::transpose(glm::make_mat4(&assimpMat.a1)); // Convert to normal glm::mat4
	}


	inline glm::vec3 AssimpToGLMVec3(const aiVector3D& v) { return glm::vec3(v.x, v.y, v.z); }

	MyMath::Quaternion Slerp(const MyMath::Quaternion& begin, const MyMath::Quaternion& end, float factor);
	glm::vec3 Slerp(const glm::vec3& begin, const glm::vec3& end, float factor);

	glm::vec3 Lerp(const glm::vec3& begin, const glm::vec3& end, float factor);
	float Lerp(float begin, float end, float factor);

	float ExpoLerp(float begin, float end, float factor);
	glm::vec3 ExpoLerp(const glm::vec3& begin, const glm::vec3& end, float factor);

	struct iSlerp {
	private:
		int Count = -1;
		int currCount = 0;
		MyMath::Quaternion qc;
		MyMath::Quaternion qk;
	public:
		void iSlerpInit(const MyMath::Quaternion& begin, const MyMath::Quaternion& end, int count)
		{
			// iSlerp constant
			const float alpha = acos(begin.Dot(end));
			const float beta = alpha / 2;
			const glm::vec3 u = (begin.s * begin.v - end.s * begin.v + glm::cross(begin.v, end.v)) / sin(alpha);
			qc = MyMath::Quaternion(cos(beta), sin(beta) * u);
			qk = begin;
			currCount = 0;
			Count = count;
		}

		bool iSlerpStep()
		{
			if (++currCount < Count)
			{
				qk *= qc;
				return true;
			}
			else
			{
				return false;
			}
		}
		MyMath::Quaternion GetQuat() const
		{
			return qk;
		}
	};

	struct iVQS {
	private:
		glm::mat3 m_Nc{}; // incremental matrix
		glm::vec3 m_Vk{}, m_Vc{}; // Incremental Translation
		int Count = -1;    // Number of interpolation steps
		int currCount = 0;
	public:
		void iVQSInit(const MyMath::VQS& begin, const MyMath::VQS& end, int count)
		{
			// iLerp constant
			const glm::vec3 vc = (end.v - begin.v) / count;

			// iSlerp constant
			const float alpha = acos(begin.q.Dot(end.q));
			const float beta = alpha / 2;
			const glm::vec3 u = (begin.s * begin.v - end.s * begin.v + glm::cross(begin.v, end.v)) / sin(alpha);
			const MyMath::Quaternion qc(cos(beta), sin(beta) * u);

			// iELerp constant
			const float sc = pow((end.s / begin.s), (1.0f / count));
			const glm::mat3 Mc = glm::mat3(qc.ToMat4());

			m_Nc = sc * Mc;
			m_Vc = vc;
			m_Vk = begin.v;
			currCount = 0;
			Count = count;
		}

		// This function steps to the next interpolated VQS and
		// increment the count. It is called once and only once
		// for each transformation frame. The function returns
		// FALSE at the end of n iterations or TRUE otherwise. 
		bool iVQSStep()
		{
			if (++currCount < Count)
			{
				m_Vk += m_Vc;
				return true;
			}
			else
			{
				return false;
			}
		}

		// This function returns a transformed vector based on
		// rk from previous step.
		glm::vec3 iVQSTransform(const glm::vec3& rk) const
		{
			return m_Nc * rk + m_Vk;
		}

		MyMath::VQS GetVQSIter() const
		{
			glm::mat4 res{ m_Nc };
			res[3] = glm::vec4(m_Vk, 1.0f);
			return MyMath::VQS(res);
		}
	};

	struct iLerp {
	private:
		int Count = -1;
		int currCount = 0;
		glm::vec3 vc;
		glm::vec3 vk;
	public:
		void iLerpInit(const glm::vec3& begin, const glm::vec3& end, int count)
		{
			// iLerp constant
			vc = (end - begin) / count;
			vk = begin;
			currCount = 0;
			Count = count;
		}

		bool iLerpStep()
		{
			if (++currCount < Count)
			{
				vk += vc;
				return true;
			}
			else
			{
				return false;
			}
		}
		glm::vec3 GetValue() const
		{
			return vk;
		}
	};

};


#endif //OPENGLFRAMEWORK_MYMATH_H
