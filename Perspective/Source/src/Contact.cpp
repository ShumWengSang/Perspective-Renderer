#include "stdafx.h"
#include "Contact.h"


namespace NarrowCollision
{
	// Closest point between two line segments
	static void ClosestPointOfSegments(const glm::vec3 colliding_edge[2][2], glm::vec3 point_out[2])
	{
		glm::vec3 d[2] = {
	colliding_edge[0][1] - colliding_edge[0][0],
	colliding_edge[1][1] - colliding_edge[1][0]
		};

		glm::vec3 r = colliding_edge[0][0] - colliding_edge[1][0];

		float a = glm::dot(d[0], d[0]);
		float e = glm::dot(d[1], d[1]);
		float c = glm::dot(d[0], r);
		float f = glm::dot(d[1], r);
		float b = glm::dot(d[0], d[1]);

		float t[2];
		float denom = a * e - b * b;
		if (denom > PHYSICS_TOLERANCE)
			t[0] = std::clamp((b * f - c * e) / denom, float(0), float(1));
		else
			t[0] = float(0);

		t[1] = (b * t[0] + f) / e;

		if (t[1] < float(0))
		{
			t[1] = float(0);
			t[0] = std::clamp(-c / a, float(0), float(1));
		}
		else if (t[1] > float(1))
		{
			t[1] = float(1);
			t[0] = std::clamp((b - c) / a, float(0), float(1));
		}

		point_out[0] = colliding_edge[0][0] + d[0] * t[0];
		point_out[1] = colliding_edge[1][0] + d[1] * t[1];
	}
	namespace SAT
	{
		enum class SeparatingAxis : int {
			Box0XxBox1X = 0,
			Box0XxBox1Y,
			Box0XxBox1Z,
			Box0YxBox1X,
			Box0YxBox1Y,
			Box0YxBox1Z,
			Box0ZxBox1X,
			Box0ZxBox1Y,
			Box0ZxBox1Z,

			Box0X,
			Box0Y,
			Box0Z,
			Box1X,
			Box1Y,
			Box1Z,

			Count,
			Unknown,
			Start = Box0XxBox1X,
		};

		static constexpr int ColumnIndices[9][2] = {
	{ 0, 0 }, // SeparatingAxis::Box0XxBox1X (== 0) -> box0->Orientation().Column(0) x box1->Orientation().Column(0)
	{ 0, 1 }, // SeparatingAxis::Box0XxBox1Y (== 1) -> box0->Orientation().Column(0) x box1->Orientation().Column(1)
	{ 0, 2 }, // SeparatingAxis::Box0XxBox1Z (== 2) -> :
	{ 1, 0 }, // SeparatingAxis::Box0YxBox1X (== 3)
	{ 1, 1 }, // SeparatingAxis::Box0YxBox1Y (== 4)
	{ 1, 2 }, // SeparatingAxis::Box0YxBox1Z (== 5)
	{ 2, 0 }, // SeparatingAxis::Box0ZxBox1X (== 6)
	{ 2, 1 }, // SeparatingAxis::Box0ZxBox1Y (== 7) -> :
	{ 2, 2 }, // SeparatingAxis::Box0ZxBox1Z (== 8) -> box0->Orientation().Column(2) x box1->Orientation().Column(2)
		};

		//Current candidate value of penetration depth along best_axis_id
		struct SATPenetration {
			glm::vec3 axis = glm::vec3(0);
			SeparatingAxis axis_id = SeparatingAxis::Unknown;
			float depth = PHYSICS_REAL_MAX;
		};

		// Current evaluation of SAT best axis and penetrations
		struct SATEvalStatus {
			glm::vec3 best_axis = glm::vec3(0);
			SeparatingAxis best_axis_id = SeparatingAxis::Start;
			std::array<SATPenetration, static_cast<int>(SeparatingAxis::Count)> penetration;
		};

		// Store the current SAT Context
		struct SATContext {
			SeparatingAxis current_axis_id;
			glm::vec3 h[2];
			glm::quat RT[2];
			glm::vec3 distance;
		};


		static float HalfExtentOnAxis(const glm::vec3& axis, const glm::vec3& h, const glm::quat& RT)
		{
			glm::vec3 axis_boxlocal = RT * axis;
			return
				std::abs(axis_boxlocal.x) * h.x +
				std::abs(axis_boxlocal.y) * h.y +
				std::abs(axis_boxlocal.z) * h.z;
		}

		static float OverlapAlongAxis(const glm::vec3& axis, const glm::vec3 h[2], const glm::quat RT[2], const glm::vec3& distance)
		{
			float r0 = HalfExtentOnAxis(axis, h[0], RT[0]);
			float r1 = HalfExtentOnAxis(axis, h[1], RT[1]);
			float D = std::abs(glm::dot(axis, distance));

			return r0 + r1 - D;
		}


		static bool SeparatedOnAxis(glm::vec3&& axis, const SATContext& ctx, SATEvalStatus& status)
		{
			if (glm::length2(axis) >= PHYSICS_TOLERANCE)
			{
				axis = glm::normalize(axis);
				float current_penetration = OverlapAlongAxis(axis, ctx.h, ctx.RT, ctx.distance);
				status.penetration[static_cast<int>(ctx.current_axis_id)].depth = current_penetration;
				status.penetration[static_cast<int>(ctx.current_axis_id)].axis_id = ctx.current_axis_id;
				status.penetration[static_cast<int>(ctx.current_axis_id)].axis = axis;

				if (current_penetration <= PHYSICS_TOLERANCE)
					return true;
				if (current_penetration < status.penetration[static_cast<int>(status.best_axis_id)].depth)
				{
					status.best_axis = axis;
					status.best_axis_id = ctx.current_axis_id;
				}
			}
			return false;
		}

		static bool CheckSeparationStatus(SAT::SATContext& ctx, const glm::quat* quats, SAT::SATEvalStatus& status)
		{
			bool separated = false;
			glm::mat3 R[2]{ glm::toMat3(quats[0]), glm::toMat3(quats[1]) };

			// SAT using local axes of Box0
			ctx.current_axis_id = SeparatingAxis::Box0X;
			separated = SeparatedOnAxis(glm::vec3(R[0][0]), ctx, status);
			if (separated) return false;
			ctx.current_axis_id = SeparatingAxis::Box0Y;
			separated = SeparatedOnAxis(glm::vec3(R[0][1]), ctx, status);
			if (separated) return false;
			ctx.current_axis_id = SeparatingAxis::Box0Z;
			separated = SeparatedOnAxis(glm::vec3(R[0][2]), ctx, status);
			if (separated) return false;

			// SAT using local axes of Box1
			ctx.current_axis_id = SeparatingAxis::Box1X;
			separated = SeparatedOnAxis(glm::vec3(R[1][0]), ctx, status);
			if (separated) return true;
			ctx.current_axis_id = SeparatingAxis::Box1Y;
			separated = SeparatedOnAxis(glm::vec3(R[1][1]), ctx, status);
			if (separated) return true;
			ctx.current_axis_id = SeparatingAxis::Box1Z;
			separated = SeparatedOnAxis(glm::vec3(R[1][2]), ctx, status);
			if (separated) return true;

			// SAT using cross product from the local axes of each boxes
			ctx.current_axis_id = SeparatingAxis::Box0XxBox1X;
			separated = SeparatedOnAxis(glm::cross(R[0][0], R[1][0]), ctx, status);
			if (separated) return true;
			ctx.current_axis_id = SeparatingAxis::Box0XxBox1Y;
			separated = SeparatedOnAxis(glm::cross(R[0][0], R[1][1]), ctx, status);
			if (separated) return true;
			ctx.current_axis_id = SeparatingAxis::Box0XxBox1Z;
			separated = SeparatedOnAxis(glm::cross(R[0][0], R[1][2]), ctx, status);
			if (separated) return true;
			ctx.current_axis_id = SeparatingAxis::Box0YxBox1X;
			separated = SeparatedOnAxis(glm::cross(R[0][1], R[1][0]), ctx, status);
			if (separated) return true;
			ctx.current_axis_id = SeparatingAxis::Box0YxBox1Y;
			separated = SeparatedOnAxis(glm::cross(R[0][1], R[1][1]), ctx, status);
			if (separated) return true;
			ctx.current_axis_id = SeparatingAxis::Box0YxBox1Z;
			separated = SeparatedOnAxis(glm::cross(R[0][1], R[1][2]), ctx, status);
			if (separated) return true;
			ctx.current_axis_id = SeparatingAxis::Box0ZxBox1X;
			separated = SeparatedOnAxis(glm::cross(R[0][2], R[1][0]), ctx, status);
			if (separated) return true;
			ctx.current_axis_id = SeparatingAxis::Box0ZxBox1Y;
			separated = SeparatedOnAxis(glm::cross(R[0][2], R[1][1]), ctx, status);
			if (separated) return true;
			ctx.current_axis_id = SeparatingAxis::Box0ZxBox1Z;
			separated = SeparatedOnAxis(glm::cross(R[0][2], R[1][2]), ctx, status);
			if (separated) return true;

			return false;
		}
	}

	namespace GJK
	{
		glm::vec3 FurthestVertexAlongAxis(const glm::vec3& axis, const glm::vec3& h, const glm::quat& R, const glm::quat& RT, const glm::vec3& P)
		{
			glm::vec3 axis_boxlocal = glm::normalize(RT * axis);

			auto Sign = [](float v) -> float { return v < -PHYSICS_TOLERANCE ? -1.f : 1.f; };

			const glm::vec3 FurthestVertexLocal(Sign(axis_boxlocal.x) * h.x, Sign(axis_boxlocal.y) * h.y, Sign(axis_boxlocal.z) * h.z);

			return R * FurthestVertexLocal + P;
		}
	}

	static void BuildContact(const SAT::SeparatingAxis axis_id, const Rigidbody* box0, const Rigidbody* box1, const SAT::SATContext& ctx,
		const SAT::SATPenetration& penetration, Contact* contact_out)
	{
		using namespace SAT;
		// Set the normal
		contact_out->Normal = glm::normalize(penetration.axis);

		// Based on the axis of penetration, determine the position, penetration depth, and direction of normal
		switch (axis_id)
		{
			// A vertex in Box1 is touching the face of box0
		case SeparatingAxis::Box0X:
		case SeparatingAxis::Box0Y:
		case SeparatingAxis::Box0Z:
		{
			// Normal points from box1 to box0
			// Does it matter if I normalize here?
			if (glm::dot(glm::normalize(ctx.distance), contact_out->Normal) >= 0)
			{
				contact_out->Normal *= -1;
			}

			// Determine position + penetration depth through GJK
			contact_out->Position = GJK::FurthestVertexAlongAxis(-contact_out->Normal,
				ctx.h[1], box1->orientation, ctx.RT[1], box1->position);
			contact_out->PenetrationDepth = penetration.depth;
			break;
		}
		// A vertex in Box0 is touching the face of box1
		case SeparatingAxis::Box1X:
		case SeparatingAxis::Box1Y:
		case SeparatingAxis::Box1Z:
		{
			// Normal points from box1 to box0
			if (glm::dot(glm::normalize(ctx.distance), contact_out->Normal) >= 0)
			{
				contact_out->Normal *= -1;
			}

			// Determine position + penetration depth through GJK
			contact_out->Position = GJK::FurthestVertexAlongAxis(-contact_out->Normal,
				ctx.h[0], box0->orientation, ctx.RT[0], box0->position);
			contact_out->PenetrationDepth = penetration.depth;
			break;
		}
		// Both boxes are touching each other's edges
		default:
		{
			glm::quat R[2]{ box0->orientation, box1->orientation };
			// Point from box1 to box0
			if (glm::dot(glm::normalize(ctx.distance), contact_out->Normal))
			{
				contact_out->Normal *= -1;
			}
			// Estimate touching position by first identifying touching edge
			// calculate closest points on these edges
			// take the midpoint of the points as the touching position
			const int* colIndex = ColumnIndices[static_cast<int>(axis_id)];
			const glm::vec3 best_axis_localbox[2] = {
				ctx.RT[0] * contact_out->Normal,
				ctx.RT[1] * contact_out->Normal
			};
			glm::vec3 midpoint_on_colliding_edge[2] = {
				glm::vec3(0), glm::vec3(0)
			};

			for (int i = 0; i < 3; ++i)
			{
				if (i != colIndex[0])
				{
					midpoint_on_colliding_edge[0][i] = best_axis_localbox[0][i] < 0 ? ctx.h[0][i] : -ctx.h[0][i];
				}
				if (i != colIndex[1])
				{
					midpoint_on_colliding_edge[1][i] = best_axis_localbox[1][i] < 0 ? ctx.h[1][i] : -ctx.h[1][i];
				}
			}

			// convert to the world coordinate system
			midpoint_on_colliding_edge[0] = R[0] * midpoint_on_colliding_edge[0] + box0->position;
			midpoint_on_colliding_edge[1] = R[1] * midpoint_on_colliding_edge[1] + box1->position;

			// The end points of a colliding edge can be found at positions h (and -h) away from the midpoint.
			glm::mat3 R_mat[2] = { glm::toMat3(R[0]), glm::toMat3(R[1]) };
			const glm::vec3 colliding_edge[2][2] = {
		{
			midpoint_on_colliding_edge[0] + ctx.h[0][colIndex[0]] * R_mat[0][colIndex[0]],
			midpoint_on_colliding_edge[0] - ctx.h[0][colIndex[0]] * R_mat[0][colIndex[0]],
			},
		{
			midpoint_on_colliding_edge[1] + ctx.h[1][colIndex[1]] * R_mat[1][colIndex[1]],
			midpoint_on_colliding_edge[1] - ctx.h[1][colIndex[1]] * R_mat[1][colIndex[1]],
			},
			};
			// Out edge
			glm::vec3 pointOut[2];
			ClosestPointOfSegments(colliding_edge, pointOut);
			// Tweak contact out position by taking average
			contact_out->Position = 0.5f * (pointOut[0] + pointOut[1]);
			contact_out->PenetrationDepth = penetration.depth * 0.5f;
		}
		break;
		}

		contact_out->RelativeBodyPosition[0] = contact_out->Position - box0->position;
		contact_out->RelativeBodyPosition[1] = contact_out->Position - box1->position;
		contact_out->Body[0] = const_cast<Rigidbody*>(box0);
		contact_out->Body[1] = const_cast<Rigidbody*>(box1);
	}

	bool Detect(Rigidbody* box0, Rigidbody* box1, std::vector<Contact>& contacts_out)
	{
		/*
		 * Gather stats
		 */
		const glm::vec3 h[2] = { std::get<1>(box0->colliders[0].aabb.GetCenterHalfExtents()), std::get<1>(box1->colliders[0].aabb.GetCenterHalfExtents()) };
		const glm::quat R[2] = { box0->orientation, box1->orientation };
		const glm::quat RT[2] = { box0->inverseOrientation, box1->inverseOrientation };
		const glm::vec3 P[2] = { box0->position, box1->position };
		const glm::vec3 distance = P[1] - P[0];

		// Setup the SAT context
		SAT::SATContext ctx = {
			SAT::SeparatingAxis::Unknown,
		{ h[0], h[1] },
		{ RT[0], RT[1] },
	P[1] - P[0]
		};

		SAT::SATEvalStatus status;

		// Meat of SAT
		if (SAT::CheckSeparationStatus(ctx, R, status)) {
			return false;
		}
		int best_count = 1;
#if 1
		// Sorts penetration
		std::stable_sort(status.penetration.begin(), status.penetration.end(),
			[](auto const& lhs, auto const& rhs) {
				return lhs.depth < rhs.depth;
			}
		);

		// Get a list of the best penetration
		best_count = std::count_if(status.penetration.begin(), status.penetration.end(),
			[&](auto& pen) {
				return pen.depth == status.penetration[0].depth;
			}
		);
#endif
		Contact contact_out;
		for (int i = 0; i < best_count; ++i) {
			BuildContact(status.penetration[i].axis_id, box0, box1, ctx, status.penetration[i], &contact_out);
			contacts_out.emplace_back(contact_out);
		}

		return true;
	}
}

Arbiter::Arbiter(Rigidbody* a, Rigidbody* b)
{
	if (a < b)
	{
		body1 = a;
		body2 = b;
	}
	else
	{
		body1 = b;
		body2 = a;
	}

	friction = sqrtf(body1->friction * body2->friction);

	NarrowCollision::Detect(body1, body2, this->contacts);
}

void Arbiter::Update(const std::vector<Contact>& newContacts)
{
	std::vector<Contact> mergedContacts(newContacts.size());

	for (int i = 0; i < newContacts.size(); ++i)
	{
		const Contact* newContact = &newContacts[i];
		int k = -1;
		// See if this 'new' contact already exists
		for (int j = 0; j < contacts.size(); ++j)
		{
			const Contact* oldContact = &contacts[j];
			if (newContact == oldContact)
			{
				k = j;
				break;
			}
		}

		// If it does exist
		if (k > -1)
		{
			Contact& c = mergedContacts[i];
			Contact& cOld = contacts[k];

			c = *newContact;
		}
		// If it doesn't exist
		else
		{
			mergedContacts[i] = newContacts[i];
		}
	}

	for (int i = 0; i < newContacts.size(); ++i)
		contacts[i] = mergedContacts[i];
}

void Arbiter::PreStep(float inv_dt)
{
}

void Arbiter::ApplyImpulse()
{
}

void Arbiter::ApplySolver(rbSolver* solver, float dt)
{
	for (Contact& contact : contacts)
	{
		solver->ApplyImpulse(&contact, dt);
	}
}

bool operator<(const ArbiterKey& a1, const ArbiterKey& a2)
{
	if (a1.body1 < a2.body1)
		return true;

	if (a1.body1 == a2.body1 && a1.body2 < a2.body2)
		return true;

	return false;
}
