#pragma once

#include "node.hpp"
#include "../../engine/physics/physicsEngine.hpp"
#include "../../manager/gamemanager.hpp"

struct BTIncreaseGoalTime : BTNode {
    BTIncreaseGoalTime(){};

	BTNodeStatus run(EntityContext* ectx) noexcept final {
        auto status {BTNodeStatus::success};

        auto gm = GameManager::Instance();
        gm->completeGoal(ectx->deltaTime);

        return status;
    }
};