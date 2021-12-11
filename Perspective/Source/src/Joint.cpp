#include "stdafx.h"
#include "Joint.h"
#include <glm/gtx/matrix_cross_product.hpp>

void Joint::Set(Rigidbody* body1, Rigidbody* body2, const glm::vec3& anchor)
{

	bias = {};
	P = {};
	softness = 0.0f;
	biasFactor = 0.2f;

	this->body1 = body1;
	this->body2 = body2;

	const glm::quat rot1T = body1->inverseOrientation;
	const glm::quat rot2T = body2->inverseOrientation;

	localAnchor1 = rot1T * (anchor - body1->position);
	localAnchor2 = rot2T * (anchor - body2->position);
}

void Joint::PreStep(float inv_dt)
{
	P = {};
	r1 = body1->orientation * localAnchor1;
	r2 = body2->orientation * localAnchor2;

	glm::mat3 k1(1.0f);
	k1 *= body1->inverseMass + body2->inverseMass;

	const glm::mat3 r1_tilda = glm::matrixCross3(r1);
	const glm::mat3 r2_tilda = glm::matrixCross3(r2);

	const glm::mat3 K = k1 - r1_tilda * body1->globalInverseInertiaTensor * r1_tilda
					 - r2_tilda * body2->globalInverseInertiaTensor * r2_tilda;
	M = glm::inverse(K);

	const glm::vec3 p1 = body1->position + r1;
	const glm::vec3 p2 = body2->position + r2;
	const glm::vec3 dp = p2 - p1;
	
	bias = -biasFactor * inv_dt * dp;
}

void Joint::ApplyImpulse()
{
	const glm::vec3 dv = body2->linearVelocity + glm::cross(body2->angularVelocity, r2)
				 - body1->linearVelocity - glm::cross(body1->angularVelocity, r1);

	const glm::vec3 impulse = M * (bias - dv - softness * P);

	body1->ApplyImpulse(-impulse, r1);
	body2->ApplyImpulse(impulse, r2);

	P += impulse;
}
