// CS250
// Roland Shum
// Assignment 3
// Camera.cpp
#include "../stdafx.h"
#include "Camera.h"
#include "../Affine.h"

using Radians = float;
using Degrees = float;

Camera::Camera(void) : eye_point(glm::vec4(0)), right_vector(glm::vec4(1, 0, 0, 0)),
up_vector(glm::vec4(0, 1, 0, 0)), back_vector(glm::vec4(0, 0, 0, 1)),
vp_distance(5), near_distance(0.1f), far_distance(near_distance)
{
	// Aspect ratio of 1, compile time evaluation
	constexpr Radians angle = glm::radians(90.0f / 2);
	vp_height = vp_width = tanf(angle)* vp_distance * 2;

}

Camera::Camera(const glm::vec4& E, const glm::vec4& look, const glm::vec4& rel, float fov, float aspect, float N, float F) :
	eye_point(E),
	right_vector(glm::normalize(Math::cross(look, rel))), up_vector(glm::normalize(rel)), back_vector(glm::normalize( look * -1.0f)),
    vp_distance(N), near_distance(N), far_distance(F)
{
	Degrees const FOV = fov;
	float aspectRatio = aspect;

	Radians radian = glm::radians(FOV / 2);
	vp_width = tanf(radian) * vp_distance * 2;
	vp_height = vp_width / aspectRatio;
}

glm::vec4 Camera::viewport(void) const
{
	return glm::vec4(vp_width, vp_height, vp_distance, 0);
}

Camera& Camera::zoom(float factor)
{
	this->vp_height *= factor;
	this->vp_width *= factor;
	return *this;
}

Camera& Camera::forward(float distance)
{
	glm::vec4 movementVector = this->back_vector * -distance;
	this->eye_point += movementVector;
	return *this;
}

Camera& Camera::yaw(float angle)
{
	auto rotationMatrix = Math::rotate(angle, up_vector);
	right_vector = rotationMatrix * right_vector;
	back_vector = rotationMatrix * back_vector;
	return *this;
}

Camera& Camera::pitch(float angle)
{
	auto rotationMatrix = Math::rotate(angle, right_vector);
	up_vector = rotationMatrix * up_vector;
	back_vector = rotationMatrix * back_vector;
	return *this;
}

Camera& Camera::roll(float angle)
{
	auto rotationMatrix = Math::rotate(angle, back_vector);
	right_vector = rotationMatrix * right_vector;
	up_vector = rotationMatrix * up_vector;
	return *this;
}

// Maps a canonical camera to world space (camera to world)
glm::mat4 Camera::model(const Camera& cam)
{
	return Math::affine(cam.right(), cam.up(), cam.back(), cam.eye());
}

// Maps a world to camera. Use this to view the world from camera.
glm::mat4 Camera::view(const Camera& cam)
{
	return affineInverse(Math::affine(cam.right(), cam.up(), cam.back(), cam.eye()));
}

// Perspective transformation that maps camera to standard cube. (NDC)
glm::mat4 Camera::perspective(const Camera& cam)
{
	float W = cam.viewport().x;
	float H = cam.viewport().y;
	float D = cam.viewport().z;


	glm::mat4 res(1); // Initialize to identity


	res[0][0] = 2 * D / W;
	res[1][1] = 2 * D / H;
	res[2][2] = (cam.near() + cam.far()) / (cam.near() - cam.far());
	res[3][3] = 0;



	res[2][3] = -1;
	res[3][2] = (2 * cam.near() * cam.far()) / (cam.near() - cam.far());

	return res;
}

