#pragma once

#include <cstdint>

struct weaponStats {
    uint8_t     ammo{};
    uint8_t     magSize{};
    uint16_t    totalAmmo{};
    double      reloadElapsed{}, reloadTime{};
    float       hitEnemy{};
    int         collisionType{};
    bool        infiniteAmmo{};
};