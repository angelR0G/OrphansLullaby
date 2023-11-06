#pragma once

#include "node.hpp"
#include "../../engine/physics/physicsEngine.hpp"

#define NEARBY_DISTANCE 85.f

struct BTPlayerNearby : BTNode {
    BTPlayerNearby(){};

	BTNodeStatus run(EntityContext* ectx) noexcept final {
        auto status {BTNodeStatus::fail};

        float entX {ectx->pos->x};
        float entZ {ectx->pos->z};
        float playerX {ectx->posPlayer->x};
        float playerZ {ectx->posPlayer->z};

        if( entX < playerX - NEARBY_DISTANCE || entX > playerX + NEARBY_DISTANCE ||
            entZ < playerZ - NEARBY_DISTANCE || entZ > playerZ + NEARBY_DISTANCE  ) {
            ectx->ai->behaviour = SB::AFK;
            status = {BTNodeStatus::success};
        }

        return status;
    }
};