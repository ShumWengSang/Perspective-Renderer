#include "stdafx.h"
#include "IKSolver.h"

glm::quat fromTo(const glm::vec3& from, const glm::vec3& to) {
	using namespace glm;
	vec3 f = normalize(from);
	vec3 t = normalize(to);

	if (f == t) {
		return quat();
	}
	else if (f == t * -1.0f) {
		vec3 ortho = vec3(1, 0, 0);
		if (fabsf(f.y) < fabsf(f.x)) {
			ortho = vec3(0, 1, 0);
		}
		if (fabsf(f.z) < fabs(f.y) && fabs(f.z) < fabsf(f.x)) {
			ortho = vec3(0, 0, 1);
		}

		vec3 axis = normalize(cross(f, ortho));
		return quat(axis.x, axis.y, axis.z, 0);
	}

	vec3 half = normalize(f + t);
	vec3 axis = cross(f, half);

	return quat(
		axis.x,
		axis.y,
		axis.z,
		dot(f, half)
	);
}
