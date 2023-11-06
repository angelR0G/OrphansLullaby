#pragma once

#include <cstdint>

struct CollisionObjectData {
    void*       entity{nullptr};
    uint16_t    flags{0};
};