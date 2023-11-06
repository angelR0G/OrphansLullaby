#pragma once

#include <cstdint>

struct Position3D {
    float   x {},
            y {},
            z {};
};

struct MapCluster {
    Position3D  minPos  {},
                maxPos  {};
    uint8_t     id      {};
    float       reverb  {};

    [[nodiscard]] constexpr bool isPointInside(const Position3D& point) const noexcept {
        return  point.x >= minPos.x && point.x <= maxPos.x &&
                point.y >= minPos.y && point.y <= maxPos.y && 
                point.z >= minPos.z && point.z <= maxPos.z;
    }  

    [[nodiscard]] constexpr bool isPointOutside(const Position3D& point) const noexcept {
        return  point.x < minPos.x || point.x > maxPos.x ||
                point.y < minPos.y || point.y > maxPos.y || 
                point.z < minPos.z || point.z > maxPos.z;
    }  
};