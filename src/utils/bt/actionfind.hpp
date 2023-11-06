#pragma once

#include "node.hpp"
#include "../../engine/physics/physicsEngine.hpp"

#define SEEK_DIST 200.0f

struct BTActionFind : BTNode {
    BTActionFind(){};

	BTNodeStatus run(EntityContext* ectx) noexcept final {
        auto status {BTNodeStatus::fail};

        // cambiar esto al fusionarlo con la ia actualizada
        auto distx { ectx->posPlayer->x - ectx->pos->x};
        auto distz { ectx->posPlayer->z - ectx->pos->z};
        auto distlin { std::sqrt(distx*distx + distz*distz) };

        if(distlin<=SEEK_DIST){
            ectx->ai->behaviour = SB::Seek;

            ectx->mov->tryToRun = false;

            status = BTNodeStatus::success;
        }

        return status;
    }
};