#pragma once

#include "node.hpp"
#include "../../engine/physics/physicsEngine.hpp"

struct BTCheckCurrentHealth : BTNode {
    BTCheckCurrentHealth(){};

	BTNodeStatus run(EntityContext* ectx) noexcept final {
        auto status {BTNodeStatus::fail};

        if(ectx->hp->health<=0){
            ectx->ai->behaviour = SB::AFK;      // stop moving (for movable objective)
            status = {BTNodeStatus::success};
        }

        return status;
    }
};