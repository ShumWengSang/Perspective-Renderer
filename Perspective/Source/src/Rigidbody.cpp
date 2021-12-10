#include "stdafx.h"
#include "Rigidbody.h"


void Rigidbody::UpdateOrientation(void)
{
	orientation = glm::normalize(orientation);
	inverseOrientation = glm::inverse(orientation);
}

void Rigidbody::AddCollider(Collider& newCollider)
{
	collider = newCollider;
	collider.rb = this;
	collider.aabb.collider = &collider;

	mass = collider.mass;
	position = collider.aabb.center;

	inverseMass = 1.f / mass;

	halfExtent = newCollider.aabb.halfExtents;

	glm::mat3 localInertiaTensor = collider.localInertiaTensor;

	localInverseInertiaTensor = glm::inverse(localInertiaTensor);
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

void Rigidbody::ApplyForce(const glm::vec3& f)
{
	if(fixed)
		return;
	forceAccumulator += f;
}

void Rigidbody::ApplyForce(const glm::vec3& f, const glm::vec3& at)
{
	if (fixed)
		return;
	forceAccumulator += f;
	torqueAccumulator += glm::cross(at - position, f);
}

void Rigidbody::ApplyImpulse(const glm::vec3& impulse, const glm::vec3 relativePosition)
{
	if (fixed)
		return;
	linearVelocity += inverseMass * impulse;

	const glm::vec3 L = glm::cross(relativePosition, impulse);
	angularMomentum += L;
	angularVelocity += globalInverseInertiaTensor * L;
}

void Rigidbody::UpdateVelocity(float dt)
{
	if (fixed)
		return;
	constexpr glm::vec3 gravity(0, -9.8, 0);
	if(useGravity)
		linearVelocity += dt * (gravity + inverseMass * forceAccumulator );
	linearVelocity += inverseMass * dt * forceAccumulator;
	angularMomentum += dt * torqueAccumulator;
	angularVelocity = globalInverseInertiaTensor * angularMomentum;
}

void Rigidbody::UpdatePosition(float dt)
{
	if (fixed)
		return;
	position += linearVelocity * dt;
	// Todo: hack this away later
	collider.aabb.center = position;
	
	// Integrate orientation
	const glm::vec3 axis = glm::normalize(angularVelocity);
	if(glm::all(glm::isnan(axis)))
		return;
	const float angle = glm::length(angularVelocity) * dt;
	orientation = glm::rotate(orientation, angle, axis);

	UpdateOrientation();
}

void Rigidbody::Reset()
{
	linearVelocity = glm::vec3(0);
	angularVelocity = glm::vec3(0);
	angularMomentum = glm::vec3(0);
	forceAccumulator = glm::vec3(0);
	torqueAccumulator = glm::vec3(0);

	position = glm::vec3(0);
	orientation = glm::vec3(0);
}