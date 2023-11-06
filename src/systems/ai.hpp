#pragma once

#include "../engine/types.hpp"

struct AISystem {
    void update(EntityMan& EM, PhysicsEngine& physics, float deltaTime, bool, size_t);
    void updateFlock(EntityMan& EM, Entity& e);
    void arrive(AIComponent&, MovementComponent&, BasicComponent&);
    void flee(AIComponent&, MovementComponent&, BasicComponent&, BasicComponent&);
    void seek(AIComponent&, MovementComponent&, BasicComponent&, BasicComponent&);
    void pursue(AIComponent&, MovementComponent&, BasicComponent&, BasicComponent&, MovementComponent*);
    
    void avoidCollision(BasicComponent&, PhysicsEngine&, MovementComponent&);
    void updateOrientationAvoid(BasicComponent&);
    void afk(MovementComponent&);

    void addNewTarget(size_t);
    void removeTarget(size_t);
    void changeTarget(uint8_t, size_t);
    void clearTargets();
    void respawnEnemy(Entity*, BasicComponent&, BasicComponent*, AIComponent&, PhysicsEngine&);
    void eventRespawnEnemy(int);

    uint8_t getEnemyType(Entity*);
    std::vector<Entity*> getIdTargets(EntityMan& EM);

    private:
    
        std::vector<size_t> idTargets {};
};