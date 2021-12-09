#include "stdafx.h"
#include "N_SquareBroadphase.h"
#include "Rigidbody.h"
#include "AABB.h"

void NSquared::Add(AABB* aabb)
{
	aabbs.emplace_back(aabb);
}

ColliderPairList& NSquared::ComputePairs()
{
	pairs.clear();

	for(auto i = aabbs.begin(); i != aabbs.end(); ++i)
	{
		for(auto j = i + 1; j != aabbs.end(); ++j)
		{
			AABB& A = **i;
			AABB& B = **j;
			Collider* colliderA = A.collider;
			Collider* colliderB = B.collider;
			Rigidbody* bodyA = colliderA->rb;
			Rigidbody* bodyB = colliderB->rb;

			// skip same-body collision
			if (bodyA == bodyB)
				continue;
			// Skip no mass
			if(bodyA->inverseMass == 0.f && bodyB->inverseMass == 0.f)
				continue;

			// add collider pair
			if (AABB::Collides(A, B))
				pairs.push_back(
					std::make_pair(A.collider, B.collider));
		}
	}
	return pairs;
}