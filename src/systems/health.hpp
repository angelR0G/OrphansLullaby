#pragma once

#include "../engine/types.hpp"

struct playerStats;

struct HealthSystem {
    void update(EntityMan& EM, float deltaTime, playerStats& pStats);
    uint8_t checkEnemyType(Entity& e);
    uint8_t checkEnemyHeadShot(Entity& e);

    private:
        void deathAnimation(Entity& e, EntityMan& EM);
        static uint8_t healthCurrentId;
};