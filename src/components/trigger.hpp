#pragma once

#include "../engine/physics/collisionObject.hpp"
#include "../manager/eventCodes.hpp"
#include "../utils/WeaponData.hpp"

struct TriggerComponent {
    bool    bRemove{false};
    bool    bActive{true};
    uint8_t inside{0};
    bool    playerInside    {false};
    bool    playerEnter     {false};
    bool    playerExit      {false};
    bool    prevPlayerInside{false};

    uint16_t eventCode{EVENT_DEFAULT};
    WeaponData weaponData;
    int position;
    CollisionObject collision;
};