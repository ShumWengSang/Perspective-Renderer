#include "stdafx.h"
#include "Physics.h"

#include <TransformSystem.h>

void Physics::Update(float deltatime, int iterations)
{
	const float dt = deltatime / iterations;
	// pre process
	contacts.clear();

	// Update broadphase and arbiters
	for (int i = 0; i < iterations; ++i)
	{
		// Update inertia tensor
		for(const auto rb : rigidbodies)
		{
			rb->UpdateInvInertialWorld();
		}

		// Broadphase n^2
		for(auto iterbody1 = rigidbodies.begin(); iterbody1 != rigidbodies.end(); ++iterbody1)
		{
			for(auto iterbody2 = iterbody1 + 1; iterbody2 != rigidbodies.end(); ++iterbody2)
			{
				std::vector<Contact> out;
				// Narrow collision
				if(NarrowCollision::Detect(*iterbody1, *iterbody2, out))
				{
					for (auto contactIter = out.begin(); contactIter != out.end(); ++contactIter)
					{
						auto it = std::find_if(contacts.begin(), contacts.end(),
							[this, &contactIter](const Contact& ci) {
								return glm::length2(contactIter->Position - ci.Position) <= 0.02 ? true : false;
							});
						if (it == contacts.end())
							contacts.push_back(*contactIter);
					}
					ImGui::Text("Colliding!");
				}
			}
		}

		// Integrate Force to velocity
		for (auto& rigidbodyPtr : rigidbodies)
		{
			auto& body = *rigidbodyPtr;

			body.UpdateVelocity(dt);

		}

		// Collision response
		for(auto& contact : contacts)
		{
			solver.ApplyImpulse(&contact,dt);
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