#pragma once

#include <reactphysics3d/collision/RaycastInfo.h>
#include <reactphysics3d/body/CollisionBody.h>
#include "collisionObjectData.hpp"

class RayCast : public reactphysics3d::RaycastCallback { 
    public: 
    reactphysics3d::decimal notifyRaycastHit(const reactphysics3d::RaycastInfo& info) {
        using namespace reactphysics3d;

        // Save if there was a hit closer to the origin
        if(!collision || info.hitFraction < hitFraction) {
            // Check if the collision object has the mask bits
            CollisionObjectData* data = static_cast<CollisionObjectData*>(info.body->getUserData());
            
            if(filterMask == 0 || (data->flags & filterMask)) {
                collision   = true;

                // Save hit point coordinates
                Vector3 hitPoint = info.worldPoint;
                hitX    = hitPoint.x;
                hitY    = hitPoint.y;
                hitZ    = hitPoint.z;

                // Save normal at hit surface
                Vector3 normals = info.worldNormal;
                normalX = normals.x;
                normalY = normals.y;
                normalZ = normals.z;

                // Save other parameters
                hitFraction = info.hitFraction;
                CollisionObjectData* data = static_cast<CollisionObjectData*>(info.body->getUserData());
                entity      = data->entity;
                bodyflags   = data->flags;
                colliderflags = *static_cast<uint8_t*>(info.collider->getUserData());
            }
        }

        return decimal(1.0); 
    }

    void reset() {
        collision       = false;
        hitFraction     = 0.0f;
        entity          = nullptr;
        bodyflags       = 0;
        colliderflags   = 0;
    }

    uint16_t filterMask{0};
    bool    collision{false};
    float   hitX,
            hitY,
            hitZ;
    float   normalX,
            normalY,
            normalZ;
    float   hitFraction{0.0f};
    void*	entity{nullptr};
	uint16_t bodyflags{0};
    uint8_t colliderflags{0};	
};