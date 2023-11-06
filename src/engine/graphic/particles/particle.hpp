#pragma once

#include "../sceneTree/transform.hpp"

struct Particle {
    GE::Transform   particleTransform{};
    glm::vec3       velocity{};
    glm::vec3       color{};
    float           lifetime{};
    bool            dead{true};

    glm::vec3       previousPosition{};
    glm::vec3       previousColor{};
};