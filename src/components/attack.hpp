#pragma once

#include "../utils/collisionTypes.hpp"

struct AttackComponent {
    float   maxDurationAttack {},
            currentAttackTime {};
    float   damage{},
            infection{};
    uint8_t damageTo{ENEMY_COLLISION};
    std::vector<uint8_t> damagedHealthId {};
};