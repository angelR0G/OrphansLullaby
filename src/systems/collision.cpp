#include "collision.hpp"

#include "../utils/CollisionCreator.hpp"
#include "../engine/physics/physicsEngine.hpp"
#include "../engine/physics/collisionCallback.hpp"

const std::vector<std::string> collisionFiles {
    "media/maps/map1/map_data/collision.obj",
    "media/maps/map2/map_data/collision.obj"
};

void CollisionSystem::update(EntityMan& EM, float deltaTime) {
    using CList = MetaP::Typelist<CollisionComponent>;
    using TList = MetaP::Typelist<>;
    EM.foreach<CList, TList>([&](Entity& e){
        CollisionComponent* collCmp = &EM.getComponent<CollisionComponent>(e);
        BasicComponent* basicCmp    = &EM.getComponent<BasicComponent>(e);

        // Update collision transform
        collCmp->collision.setTransform(basicCmp->x, basicCmp->y, basicCmp->z, basicCmp->orientation, basicCmp->verticalOrientation);
    });

    // Check collisions
    CollisionDetect callbackCollision;
    phy_engine->testCollision(callbackCollision);
}

bool CollisionSystem::canAttackDamage(AttackComponent* aCmp, HealthComponent* hCmp) {
    bool attack {true};

    // Check all entities damaged by the attack
    for(size_t i=0; i<aCmp->damagedHealthId.size() && attack; ++i) {
        // If has been attack previously, cannot receive damage
        if(aCmp->damagedHealthId[i] == hCmp->healthId) attack = false;
    }

    return attack;
}

void CollisionSystem::initPhysics(PhysicsEngine* phy){
    phy_engine = phy;
}

void CollisionSystem::deleteData(CollisionComponent& coll_cmp){
    phy_engine->deleteCollisionObject(&coll_cmp.collision);
}

void CollisionSystem::loadMapCollisions(uint8_t mapIndex) {
    clearCollisionData();
    generateMapCollisions(collisionFiles[mapIndex], *phy_engine, mapCollisions);
}

void CollisionSystem::clearCollisionData() {
    for(auto& col : mapCollisions) {
        phy_engine->deleteCollisionObject(&col);
    }

    mapCollisions.clear();
}