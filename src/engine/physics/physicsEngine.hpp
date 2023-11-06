#pragma once

#include <reactphysics3d/engine/PhysicsCommon.h>
#include "collisionObject.hpp"
#include "rayCastCallback.hpp"
#include <vector>
#include "../types.hpp"

struct GraphicEngine;
struct CollisionDetect;

struct PhysicsEngine {
    PhysicsEngine(bool);
    ~PhysicsEngine();

    // Add collision
    void addCollisionObject(CollisionComponent*, Entity&, uint16_t);
    void addCollisionObject(TriggerComponent*, Entity&, uint16_t);
    CollisionObject addCollisionObject(uint16_t, float, float, float, float, float);

    // Add colliders
    void addBoxColliderToObject(CollisionObject* object, float x=0.0, float y=0.0, float z=0.0, float w=1.0, float h=1.0, float d=1.0, uint8_t flag = 0);
    void addCapsuleColliderToObject(CollisionObject* object, float x=0.0, float y=0.0, float z=0.0, float radius=1.0, float height=1.0, uint8_t flag = 0);
    void addSphereColliderToObject(CollisionObject* object, float x=0.0, float y=0.0, float z=0.0, float radius=1.0, uint8_t flag = 0);

    void deleteCollisionObject(CollisionObject*);
    bool testOverlap(CollisionObject& c1, CollisionObject& c2);
    void testCollision(CollisionObject& c1, CollisionObject& c2, CollisionDetect&);
    void testCollision(CollisionDetect&);
    void debugPhysics(const float&, const float&, const float&);
    void raycast(RayCast&, float, float, float, float, float, float);
    void freePhysicsData();

    void enableDebug(bool) noexcept;

    private:
        void addColliderToObject(CollisionObject*, reactphysics3d::CollisionShape*, float, float, float, uint8_t);
        [[nodiscard]] bool closeToPlayer(const float&, const float&, const float&, const float&, const float&, const float&);

        reactphysics3d::PhysicsCommon phyCommon;
        reactphysics3d::PhysicsWorld* phyWorld{nullptr};
        reactphysics3d::DebugRenderer* debuger{nullptr};

        bool debugPhysicsEngine {false};
        // Debug arrays
        reactphysics3d::Vector3 hitPoints;
        std::vector<reactphysics3d::Vector3> hitPointsV{};
        reactphysics3d::Vector3 startPoint;
        std::vector<reactphysics3d::Vector3> startPointV{};
        reactphysics3d::Vector3 finalPoint;
        std::vector<reactphysics3d::Vector3> finalPointV{};
        bool collisionDetected{false};
        bool debugRaycast{false};
        GraphicEngine* device{nullptr};
};