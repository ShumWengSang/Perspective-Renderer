#include "stdafx.h"
#include "Physics.h"

#include <TransformSystem.h>

void Physics::Update(float dt, int iterations)
{
	// Update broadphase and arbiters
	for (int i = 0; i < iterations; ++i)
	{
		// Update inertia tensor
		for(const auto rb : rigidbodies)
		{
			rb->UpdateInvInertialWorld();
		}

		// broadphase.Update();
		// ColliderPairList& colliderPairs = broadphase.ComputePairs();

		for (const ColliderPair& pair : colliderPairs)
		{
			Arbiter newArbiter(pair.first->rb, pair.second->rb);
			ArbiterKey key(pair.first->rb, pair.second->rb);
			// If we have contacts
			if (!newArbiter.contacts.empty())
			{
				// Add to arbiter list

				auto iter = arbiters.find(key);
				if (iter == arbiters.end())
				{
					arbiters.insert(std::make_pair(key, newArbiter));
				}
				else
				{
					iter->second.Update(newArbiter.contacts);
				}
			}
			else
			{
				arbiters.erase(key);
			}
		}

		// Integrate Force to velocity
		for (auto& rigidbodyPtr : rigidbodies)
		{
			auto& body = *rigidbodyPtr;

			body.UpdateVelocity(dt);

		}

		// Collision response
		for(auto& arbiter : arbiters)
		{
			arbiter.second.ApplySolver(&solver, dt);
		}

		// Integrate velocity -> position
		for (auto& rigidbodyPtr : rigidbodies)
		{
			auto& body = *rigidbodyPtr;

			body.UpdatePosition(dt);
		}
	}

	// Zero out forces
	for(const auto rb : rigidbodies)
	{
		// Zero out accumuated forces
		rb->forceAccumulator = glm::vec3(0);
		rb->torqueAccumulator = glm::vec3(0);
	}

}

bool Physics::RegisterRigidbody(Rigidbody* rb)
{
	auto iter = std::find(rigidbodies.begin(), rigidbodies.end(), rb);
	if(iter == rigidbodies.end())
	{
		rigidbodies.emplace_back(rb);
		broadphase.Add(&rb->collider.aabb);
		return true;
	}
	return false;
}
