#pragma once

#include "node.hpp"
#include "../../engine/physics/physicsEngine.hpp"
#include "../collisionTypes.hpp"

#define MAX_DIST    200
#define ENEMY_EYE   14
#define PLAYER_EYE  8

struct BTCheckTargetVisible : BTNode {
    BTCheckTargetVisible(){};

	BTNodeStatus run(EntityContext* ectx) noexcept final {
        auto status {BTNodeStatus::fail};

        auto distx { ectx->posPlayer->x - ectx->pos->x};
        auto distz { ectx->posPlayer->z - ectx->pos->z};
        auto distlin { std::sqrt(distx*distx + distz*distz) };

        if(distlin<=MAX_DIST){
            RayCast ray;
            ray.filterMask = SOLID_COLLISION | PLAYER_COLLISION | DESTROYABLE_COLLISION | BARREL_COLLISION | NO_SIGHT_COLLISION;
            ectx->physics->raycast(ray, ectx->pos->x, ectx->pos->y+ENEMY_EYE, ectx->pos->z, ectx->posPlayer->x, ectx->posPlayer->y+PLAYER_EYE, ectx->posPlayer->z);
            
            if(ray.collision && (ray.bodyflags & (PLAYER_COLLISION | DESTROYABLE_COLLISION | BARREL_COLLISION))){
                ectx->nav->path.clear();
                status = BTNodeStatus::success;
            }
        }

        return status;
    }
};