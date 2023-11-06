#pragma once

#define FLEE_DIST_MIN 40.0f
#define FLEE_DIST_MAX 60.0f

struct BTActionFlee: BTNode {
    BTActionFlee(){};

	BTNodeStatus run(EntityContext* ectx) noexcept final {
        auto status {BTNodeStatus::fail};

        // cambiar esto al fusionarlo con la ia actualizada
        auto distx { ectx->posPlayer->x - ectx->pos->x};
        auto distz { ectx->posPlayer->z - ectx->pos->z};
        auto distlin { std::sqrt(distx*distx + distz*distz) };

        if(distlin>=FLEE_DIST_MIN && distlin<=FLEE_DIST_MAX){
            ectx->ai->behaviour = SB::Flee;

            status = BTNodeStatus::success;
        }

        return status;
    }
};