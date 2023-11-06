#pragma once

#include <reactphysics3d/body/CollisionBody.h>
#include "collisionObjectData.hpp"

#define MAX_COLLISIONS 700

struct CollisionObject {
    CollisionObject();

    ~CollisionObject();

    void setCollisionObject(reactphysics3d::CollisionBody*);
    void setTransform(float, float, float, float, float);
    void setRotation(float, float, float);
    void deleteCollisionData();
    CollisionObjectData* getUserData();
    void setCollisionData(void*, uint16_t);
    void updateEntity(void*);
    uint8_t* setFlags(uint8_t);

    reactphysics3d::CollisionBody* getCollisionObject();

    private:
        reactphysics3d::CollisionBody*  collisionObject{nullptr};
        CollisionObjectData*            coll_data{nullptr};

        
        static std::array<CollisionObjectData, MAX_COLLISIONS> data;
        static std::array<uint8_t, 5> flags;
        static size_t freespace;
};