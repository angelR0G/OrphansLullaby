#pragma once

#include "node.hpp"
#include "../../engine/physics/physicsEngine.hpp"
#include "../waypoint.hpp"

struct BTCalculateWaypoint : BTNode {
    BTCalculateWaypoint(){};
    
	BTNodeStatus run(EntityContext* ectx) noexcept final {
        auto status {BTNodeStatus::fail};

        if(ectx->nav->path.empty()){
            if(ectx->nav->needToUpdate == NEED_TO_UPDATE_FALSE){
                ectx->nav->needToUpdate = NEED_TO_UPDATE_TRUE;
            }

            status = BTNodeStatus::running;
            //ai.behaviour = SB::AFK;
        }
        else{
            if(!ectx->ai->tactive){
                Waypoint_t* wp = ectx->nav->path[0];

                ectx->ai->tx = wp->x;
                ectx->ai->ty = wp->y;
                ectx->ai->tz = wp->z;
                ectx->ai->tactive = true;

                // If path is long, enemy run
                if(ectx->mov != nullptr) {
                    if(ectx->nav->path.size() >= 5) 
                        ectx->mov->tryToRun = true;
                    else 
                        ectx->mov->tryToRun = false;
                }
            }
            status = BTNodeStatus::success;
        }

        return status;
    }
};