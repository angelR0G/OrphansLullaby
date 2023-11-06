#pragma once

#include "../engine/types.hpp"

#define INVULNERABILITY_TIME  2
#define SOUND_TIME            10

struct CollisionDetect;

struct CollisionSystem {
    void update(EntityMan& EM, float deltaTime);
    void deleteData(CollisionComponent&);
    void initPhysics(PhysicsEngine*);
    void loadMapCollisions(uint8_t);
    void clearCollisionData();
    bool canAttackDamage(AttackComponent*, HealthComponent*);
    
    private:
        PhysicsEngine* phy_engine;
        std::vector<CollisionObject> mapCollisions;
};