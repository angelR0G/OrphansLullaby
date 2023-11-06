#include "frustrumPlane.hpp"
#include <glm/gtx/normalize_dot.hpp>

FrustrumPlane::FrustrumPlane(const glm::vec3& p1, const glm::vec3& norm)
    : normal(glm::normalize(norm)),
    distance(glm::dot(normal, p1))
{}

float FrustrumPlane::getSignedDistanceToPlane(const glm::vec3& point) const {
    return glm::dot(normal, point) - distance;
}