#pragma once

#include "node.hpp"
#include "../../engine/physics/physicsEngine.hpp"

struct BTArriveToWaypoint : BTNode {
    BTArriveToWaypoint(){};

	BTNodeStatus run(EntityContext* ectx) noexcept final {
        auto status {BTNodeStatus::success};

        if(ectx->ai->tactive){
            if(ectx->ai->behaviour!=SB::Arrive)
                ectx->ai->behaviour = SB::Arrive;

            //status = BTNodeStatus::running;
        }
        else{
            if(!ectx->nav->path.empty()){
                ectx->nav->path.erase(ectx->nav->path.begin());
            }
            
            //status = BTNodeStatus::success;
        }

        return status;
    }
};