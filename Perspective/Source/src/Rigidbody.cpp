#include "stdafx.h"
#include "Rigidbody.h"


void Rigidbody::UpdateOrientation(void)
{
	orientation = glm::normalize(orientation);
	inverseOrientation = glm::inverse(orientation);
}

void Rigidbody::AddCollider(Collider& newCollider)
{
	colliders.emplace_back(newCollider);

	// Recompute local mass and centroid
	mass = 0.f;
	position = glm::vec3(0);

	for(const auto& collider : colliders)
	{
		mass += collider.mass;
		position += collider.mass * collider.localCentroid;
	}

	inverseMass = 1.f / mass;
	position *= inverseMass;

	// Compute local inertia tensor
	auto localInertiaTensor = glm::mat3(0);
	for (const auto& collider : colliders)
	{
		//
		const glm::vec3 r = position - collider.localCentroid;

		localInertiaTensor += collider.localInertiaTensor + collider.mass * (glm::dot(r,r)
		* glm::mat3(1.0f) - glm::outerProduct(r, r));
	}

	localInverseInertiaTensor = glm::transpose(localInertiaTensor);

	
}

const glm::vec3 Rigidbody::LocalToGlobal(const glm::vec3& p) const
{
	return orientation * p + position;
}

const glm::vec3 Rigidbody::GlobalToLocal(const glm::vec3& p) const
{
	return inverseOrientation * p + position;
}

const glm::vec3 Rigidbody::LocalToGlobalVec(const glm::vec3& v) const
{
	return orientation * v;
}

const glm::vec3 Rigidbody::GlobalToLocalVec(const glm::vec3& v) const
{
	return inverseOrientation * v;
}

void Rigidbody::ApplyForce(const glm::vec3& f, const glm::vec3& at)
{
	forceAccumulator += f;
	torqueAccumulator += glm::cross(at - position, f);
}

void Rigidbody::ApplyImpulse(const glm::vec3& impulse, const glm::vec3 relativePosition)
{
	linearVelocity += inverseMass * impulse;

	const glm::vec3 L = glm::cross(relativePosition, impulse);
	angularMomentum += L;
	angularVelocity += globalInverseInertiaTensor * L;
}

void Rigidbody::UpdateVelocity(float dt)
{
	linearVelocity += inverseMass * dt * forceAccumulator;
	angularMomentum += dt * torqueAccumulator;
	angularVelocity = globalInverseInertiaTensor * angularMomentum;
}

void Rigidbody::UpdatePosition(float dt)
{
	position += linearVelocity * dt;
		
	// Integrate orientation
	const glm::vec3 axis = glm::normalize(angularVelocity);
	const float angle = glm::length(angularVelocity) * dt;
	orientation = glm::rotate(orientation, angle, axis);

	UpdateOrientation();
}
