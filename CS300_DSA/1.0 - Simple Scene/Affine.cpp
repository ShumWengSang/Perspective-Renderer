// Roland Shum
// Assignment 1
// CS250
// Affine.cpp
#include "stdafx.h"
#include "Affine.h"

static const float EPSILON = 0.00001f;

namespace Math
{
	static bool AlmostEqual(float a, float b)
	{
		return Math::near(a, b);
		//return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * EPSILON);
	}

	static glm::mat3 createCrossProductMatrix(const glm::vec4& v)
	{
		glm::mat3 res = glm::mat3(0);

		res[0][1] = v.z;
		res[0][2] = -v.y;

		res[1][0] = -v.z;
		res[1][2] = v.x;

		res[2][0] = v.y;
		res[2][1] = -v.x;

		return res;
	}

	static glm::mat4 createDotProductMatrix(const glm::vec4& v)
	{
		glm::mat3 res = glm::mat3(1);

		res[0][0] = v.x * v.x;
		res[0][1] = v.y * v.x;
		res[0][2] = v.z * v.x;

		res[1][0] = v.x * v.y;
		res[1][1] = v.y * v.y;
		res[1][2] = v.z * v.y;

		res[2][0] = v.x * v.z;
		res[2][1] = v.y * v.z;
		res[2][2] = v.z * v.z;

		return res;
	}

	glm::vec4 point(float x, float y, float z)
	{
		return glm::vec4(x, y, z, 1);
	}

	glm::vec4 vector(float x, float y, float z)
	{
		return glm::vec4(x, y, z, 0);
	}

	bool isPoint(const glm::vec4& v)
	{
		return AlmostEqual(v.w, 1.0f);
	}

	bool isVector(const glm::vec4& v)
	{
		return AlmostEqual(v.w, 0.0f);
	}

	float angle(const glm::vec4& u, const glm::vec4& v)
	{
		glm::vec3 da = glm::normalize(u);
		glm::vec3 db = glm::normalize(v);
		return glm::degrees(glm::acos(glm::dot(da, db)));
	}

	glm::vec4 cross(const glm::vec4& u, const glm::vec4& v)
	{
		glm::vec3 a = glm::vec3(u);
		glm::vec3 b = glm::vec3(v);
		glm::vec3 product = glm::cross(a, b);
		return glm::vec4(product, 0);
	}

	glm::mat4 affine(const glm::vec4& Lx, const glm::vec4& Ly, const glm::vec4& Lz, const glm::vec4& C)
	{
		glm::mat4 result = { Lx, Ly, Lz, C };

		return result;
	}

	bool isAffine(const glm::mat4& M)
	{
		// Check if bottom row is 0 0 0 1
		return AlmostEqual(M[0][3], 0.0f) && AlmostEqual(M[1][3], 0.0f) && AlmostEqual(M[2][3], 0.0f) && AlmostEqual(M[3][3], 0.0f);
	}

	glm::mat4 rotate(float deg, const glm::vec4& v)
	{
		// Rodrqigues formula
		float radians = glm::radians(deg);
		float cosRad = glm::cos(radians);
		float sinRad = glm::sin(radians);

		glm::mat3 const firstTerm = glm::mat4(cosRad * glm::mat3(1));
		glm::mat3 const secondTerm = glm::mat4(((1 - cosRad) / glm::dot(v, v)) * createDotProductMatrix(v));
		glm::mat3 const thirdTerm = glm::mat4((sinRad / glm::length(v)) * createCrossProductMatrix(v));

		glm::mat4 result = firstTerm + secondTerm + thirdTerm;
		result[3] = glm::vec4(0, 0, 0, 1);
		return result;
	}

	glm::mat4 scale(float r)
	{
		return Math::scale(r, r, r);
	}

	glm::mat4 scale(float rx, float ry, float rz)
	{
		glm::mat4 S = glm::mat4(1);
		S[0][0] = rx;
		S[1][1] = ry;
		S[2][2] = rz;
		return S;
	}

	glm::mat4 translate(const glm::vec4& v)
	{
		glm::mat4 T = glm::mat4(1);
		T[3][0] = v.x;
		T[3][1] = v.y;
		T[3][2] = v.z;
		return T;
	}

	glm::mat4 affineInverse(const glm::mat4& A)
	{
		glm::mat3 linearTransformation = A;
		glm::vec3 translation = A[3];

		linearTransformation = glm::inverse(linearTransformation);
		translation *= (float)-1;

		glm::mat4 result = linearTransformation;
		result[3] = glm::vec4(linearTransformation * translation, 1);

		return result;
	}
}