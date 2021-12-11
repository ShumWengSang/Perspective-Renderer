#include "stdafx.h"
#include "rbSolver.h"
#include "Contact.h"

void rbSolver::PenetrationConstraintSolve(Contact* contact, float dt) const
{
	const glm::vec3 relativeVeclocity = (contact->Body[0]->linearVelocity + glm::cross(contact->Body[0]->angularVelocity, contact->RelativeBodyPosition[0]))
		- (contact->Body[1]->linearVelocity + glm::cross(contact->Body[1]->angularVelocity, contact->RelativeBodyPosition[1]));

	float impulse_mag;
	glm::vec3 impulse;

	// Apply impulse generated from collision

	{
		constexpr auto Impulse = [](Contact* contact, int index)->float
		{
			return contact->Body[index]->inverseMass +
				glm::dot(contact->Body[index]->globalInverseInertiaTensor *
					(glm::cross(
						glm::cross(contact->RelativeBodyPosition[index], contact->Normal),
						contact->RelativeBodyPosition[index])
						), contact->Normal);
		};
		float K[2] = {
			Impulse(contact, 0),
			Impulse(contact, 1)
		};
		float e = contact->Body[0]->restitution * contact->Body[1]->restitution;
		impulse_mag = -(1 + e) * (glm::dot(relativeVeclocity, contact->Normal));
		impulse_mag += biasFactor * std::max(contact->PenetrationDepth, 0.0f) / dt;
		impulse_mag /= (K[0] + K[1]);
	}
	impulse = impulse_mag * contact->Normal;
	contact->Body[0]->ApplyImpulse(impulse, contact->RelativeBodyPosition[0]);
	contact->Body[1]->ApplyImpulse(-impulse, contact->RelativeBodyPosition[1]);

	// Calculate slowdown by frection as impulse
	glm::vec3 tangent = glm::cross(contact->Normal, glm::cross(contact->Normal, relativeVeclocity));
	float tangentMag = glm::length(tangent);
	if (tangentMag > PHYSICS_TOLERANCE)
		tangent = tangent / tangentMag;
	else
		tangent = glm::vec3(0);

	{
		constexpr auto ImpulseTangent = [](Contact* contact, const glm::vec3& tangent, int index)->float
		{
			return contact->Body[index]->inverseMass +
				glm::dot(contact->Body[index]->globalInverseInertiaTensor *
					(glm::cross(
						glm::cross(contact->RelativeBodyPosition[index], tangent),
						contact->RelativeBodyPosition[index])
						), tangent);
		};
		float K[2] = {
			ImpulseTangent(contact,tangent, 0),
			ImpulseTangent(contact,tangent , 1)
		};
		// Apply friction based on coefficient of friction
		float coeff = std::min(
			std::max(std::abs(glm::dot(tangent, relativeVeclocity)), 0.f),
			contact->Body[0]->friction * contact->Body[1]->friction);
		impulse = coeff * impulse_mag * tangent;
		contact->Body[0]->ApplyImpulse(impulse, contact->RelativeBodyPosition[0]);
		contact->Body[1]->ApplyImpulse(-impulse, contact->RelativeBodyPosition[1]);
	}
}

