#pragma once

#include "node.hpp"
#include "../../engine/physics/physicsEngine.hpp"

struct BTCheckTargetYPosition : BTNode {
    BTCheckTargetYPosition(){};

	BTNodeStatus run(EntityContext* ectx) noexcept final {
        auto status {BTNodeStatus::fail};

        auto disty { ectx->posPlayer->y - ectx->pos->y};

        if(std::abs(disty)<20){
            status = BTNodeStatus::success;
        }

        return status;
    }
};