#pragma once

#include <reactphysics3d/collision/CollisionCallback.h>
#include <reactphysics3d/body/CollisionBody.h>
#include "collisionObjectData.hpp"

class CollisionDetect : public reactphysics3d::CollisionCallback { 
    public:
    
    void onContact(const reactphysics3d::CollisionCallback::CallbackData& data);

    float   hitNX{},
            hitNY{},
            hitNZ{},
            depth{};
    void*	entity;
	uint16_t flags;
};