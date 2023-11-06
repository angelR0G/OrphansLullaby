#pragma once

#include "node.hpp"
#include "../../engine/physics/physicsEngine.hpp"

#define DESTROY_DURATION    10

struct BTCheckRoutePoint : BTNode {
    BTCheckRoutePoint(){};

	BTNodeStatus run(EntityContext* ectx) noexcept final {
        auto status {BTNodeStatus::fail};

        if(ectx->ai->targetMask < ectx->ai->routeToFollow.size()){
            float currentRoute = ectx->ai->targetMask;
    
            auto distx { ectx->ai->routeToFollow[currentRoute][0] - ectx->pos->x};
            auto distz { ectx->ai->routeToFollow[currentRoute][2] - ectx->pos->z};
            auto distlin { std::sqrt(distx*distx + distz*distz) };
    
            if(distlin<=ectx->ai->arrivalRadius && ectx->hp->health>0){
                //ectx->ai->tactive = false;
                
                if(ectx->ai->routeToFollow[currentRoute][3]==1 && ectx->ai->attackCD<DESTROY_DURATION){
                    status = BTNodeStatus::running;
                    ectx->nav->needToUpdate = NEED_TO_UPDATE_FALSE;
                    ectx->nav->updateTime   = NAVIGATION_UPDATE_TIME;
                    ectx->ai->attackCD += ectx->deltaTime;
    
                    ectx->ai->behaviour = SB::AFK;
                }
                else{
                    status = BTNodeStatus::success;
                    ectx->ai->attackCD = 0;
                    ++ectx->ai->targetMask;
                }
    
            }
        }
        

        return status;
    }
};