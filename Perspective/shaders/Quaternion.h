#ifndef QUATERNIONS_H
#define QUATERNIONS_H
vec4 quaternion_mul(vec4 q1, vec4 q2) {
	return vec4(
		q2.xyz * q1.w + q1.xyz * q2.w + cross(q1.xyz, q2.xyz),
		q1.w * q2.w - dot(q1.xyz, q2.xyz)
	);
}

vec3 rotate_vector(vec4 q, vec3 v) {
	vec4 r_c = q * vec4(-1, -1, -1, 1);
	return quaternion_mul(q, quaternion_mul(vec4(v, 0), r_c)).xyz;
}

#endif 