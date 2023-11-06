#pragma once

struct BTExplosiveBarrelCD : BTNode {
    BTExplosiveBarrelCD(){};

	BTNodeStatus run(EntityContext* ectx) noexcept final {
        auto status {BTNodeStatus::running};

        //Reduce timer when its greater than 0
        if(ectx->ai->attackCD > 0){
            ectx->ai->attackCD -= ectx->deltaTime;
        }
        else{                
            //Reset duration and barrel has health again when timer reaches 0
            ectx->ai->attackCD    = EXPLOSIVE_BARREL_DURATION;
            ectx->hp->health      = BARREL_HP;

            //Restore animation to normal once the barrel is repaired
            if(ectx->animC != nullptr) {
                ectx->animC->rAnimation = static_cast<int>(RunningAnimationBarrel::RESTORE_ANIMATION);
                ectx->animC->prevAnimation = static_cast<int>(RunningAnimationBarrel::IDLE_ANIMATION);
            }
            
            status = BTNodeStatus::success;
        }

        return status;
    }
};