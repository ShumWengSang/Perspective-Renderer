#include "stdafx.h"
#include "rbSolver.h"
#include "Contact.h"

void rbSolver::PenetrationConstraintSolve(Contact* contact, float dt) const
{
	const glm::vec3 relativeVeclocity = (contact->Body[0]->linearVelocity + glm::cross(contact->Body[0]->angularVelocity, contact->RelativeBodyPosition[0]))
								-(contact->Body[1]->linearVelocity + glm::cross(contact->Body[1]->angularVelocity, contact->RelativeBodyPosition[1]));

	float impulse_mag;
	glm::vec3 impulse;

	float bias_mag;

	// Apply impulse generated from collision


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
	const float e = contact->Body[0]->restitution * contact->Body[1]->restitution;

	impulse_mag = -(1 + e) * (glm::dot(relativeVeclocity, contact->Normal));

	bias_mag = biasFactor * std::max(contact->PenetrationDepth - slop, 0.0f) / dt;
	bias_mag /= (K[0] + K[1]);

	impulse_mag /= (K[0] + K[1]);


	float temp = contact->solver_work_area.impulse;
	contact->solver_work_area.impulse = std::max(contact->solver_work_area.impulse + impulse_mag, 0.f);
	impulse_mag = contact->solver_work_area.impulse - temp;

	impulse = (impulse_mag + bias_mag) * contact->Normal;

	contact->Body[0]->ApplyImpulse(impulse, contact->RelativeBodyPosition[0]);
	contact->Body[1]->ApplyImpulse(-impulse, contact->RelativeBodyPosition[1]);

	contact->solver_work_area.biasImpulse += bias_mag;

}

void rbSolver::FrictionConstraintSolve(Contact* contact, float dt) const
{
	const glm::vec3 relativeVeclocity = (contact->Body[0]->linearVelocity + glm::cross(contact->Body[0]->angularVelocity, contact->RelativeBodyPosition[0]))
		- (contact->Body[1]->linearVelocity + glm::cross(contact->Body[1]->angularVelocity, contact->RelativeBodyPosition[1]));

	// Calculate slowdown by frection as impulse
	glm::vec3 l_tangent = glm::cross(contact->Normal, glm::cross(contact->Normal, relativeVeclocity));
	glm::vec3 l_bitangent = glm::cross(l_tangent, contact->Normal);

	auto FrictionImpulse = [contact, dt, &relativeVeclocity](glm::vec3& tangent) mutable
	{
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
			const float mag = contact->solver_work_area.impulse;
			const float limit = mag * contact->Body[0]->friction * contact->Body[1]->friction;

			// Apply friction based on coefficient of friction
			const float coeff = std::clamp(glm::dot(tangent, relativeVeclocity), -limit, limit);



			const glm::vec3 impulse = coeff * tangent;


			contact->Body[0]->ApplyImpulse(impulse, contact->RelativeBodyPosition[0]);
			contact->Body[1]->ApplyImpulse(-impulse, contact->RelativeBodyPosition[1]);
		}
	};

	FrictionImpulse(l_tangent);
	FrictionImpulse(l_bitangent);


}
