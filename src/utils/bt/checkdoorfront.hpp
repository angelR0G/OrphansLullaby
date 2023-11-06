#pragma once

#include "node.hpp"
#include "../../engine/physics/physicsEngine.hpp"

#define ROBOT_EYE   14
#define PLAYER_EYE  8
#define DISTANCE    10

struct BTCheckDoorFront : BTNode {
    BTCheckDoorFront(){};

	BTNodeStatus run(EntityContext* ectx) noexcept final {
        auto status {BTNodeStatus::fail};

        RayCast ray;
        ray.filterMask = SOLID_COLLISION | PLAYER_COLLISION | DESTROYABLE_COLLISION | BARREL_COLLISION | NO_SIGHT_COLLISION;

        if(ectx->ai->targetMask < ectx->ai->routeToFollow.size() && ectx->nav->path.size()<=1){
            float currentRoute = ectx->ai->targetMask;
            ectx->physics->raycast( ray, ectx->pos->x, ectx->pos->y+ROBOT_EYE, ectx->pos->z, 
                                    ectx->ai->routeToFollow[currentRoute][0], ectx->pos->y+ROBOT_EYE, ectx->ai->routeToFollow[currentRoute][2] );
            
            if(ray.collision && (ray.bodyflags & SOLID_COLLISION)){
                ectx->ai->behaviour = SB::AFK;
                status = BTNodeStatus::success;
            }
        }
        else if(ectx->ai->targetMask > ectx->ai->routeToFollow.size()){
            // there is no more routes availables, robot don't have to move
            status = BTNodeStatus::success;
        }
        
        return status;
    }
};