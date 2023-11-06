#pragma once

#include "../engine/entitymanager.hpp"
#include "../engine/types.hpp"

struct TriggerSystem {
    void update(EntityMan& EM, float deltaTime);
    void initPhysics(PhysicsEngine*);
    private:
        PhysicsEngine* phy_engine;
};