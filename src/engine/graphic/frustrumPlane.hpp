#pragma once

#include <glm/ext/vector_float3.hpp>

#define FRUSTRUM_NEAR_FACE   0
#define FRUSTRUM_FAR_FACE    1
#define FRUSTRUM_RIGHT_FACE  2
#define FRUSTRUM_LEFT_FACE   3
#define FRUSTRUM_TOP_FACE    4
#define FRUSTRUM_BOTTOM_FACE 5

struct FrustrumPlane
{
	FrustrumPlane() = default;

	FrustrumPlane(const glm::vec3&, const glm::vec3&);

	float getSignedDistanceToPlane(const glm::vec3& point) const;

    glm::vec3 normal = { 0.f, 1.f, 0.f }; // unit vector
	float     distance = 0.f;        // Distance with origin
};