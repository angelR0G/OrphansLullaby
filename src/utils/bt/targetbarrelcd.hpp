#pragma once

struct BTTargetBarrelCD : BTNode {
    BTTargetBarrelCD(){};

	BTNodeStatus run(EntityContext* ectx) noexcept final {
        auto status {BTNodeStatus::running};
        
        //Reduce attack time while is greater than 0
        if(ectx->ai->attackCD > 0){
            // When its maximum attack time change target of player to barrel
            if(ectx->ai->attackCD==(TARGET_BARREL_DURATION + TARGET_BARREL_CD)){
                ectx->ai->tactive = true;
                GameManager::Instance()->changePlayerTarget(ectx->ai->tactive, ectx->idEntity);
                ectx->sc->maskSounds[M_LULLABY] |= SOUND_PLAY;
            }
            // When its minor time than the cd of the barrel change target of barrel to player
            else if(ectx->ai->attackCD < TARGET_BARREL_CD && ectx->ai->tactive){

                ectx->ai->tactive = false;
                GameManager::Instance()->changePlayerTarget(ectx->ai->tactive, 0);

                //Play broken barrel animation 
                if(ectx->animC != nullptr){
                    ectx->animC->rAnimation = static_cast<int>(RunningAnimationBarrel::BREAK_ANIMATION);
                    ectx->animC->prevAnimation = static_cast<int>(RunningAnimationBarrel::IDLE_ANIMATION_BREAK);
                }
            }

            ectx->ai->attackCD -= ectx->deltaTime;
        }
        else{                
            //Reset duration and barrel has health again
            ectx->ai->attackCD    = TARGET_BARREL_DURATION + TARGET_BARREL_CD;
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