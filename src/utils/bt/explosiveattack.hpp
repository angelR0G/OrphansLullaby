#pragma once

struct BTExplosiveAttack : BTNode {
    BTExplosiveAttack(){};

	BTNodeStatus run(EntityContext* ectx) noexcept final {
        auto status {BTNodeStatus::success};

        //Attack when it reaches cooldown
        if(ectx->ai->attackCD <= ENEMY_EX_AT_DURATION){
            //Raise timer of attack duration
            ectx->ai->attackCD += ectx->deltaTime;
            
            status = BTNodeStatus::running;
        }
        else{
            // Set health to 0 to die
            ectx->hp->health = 0;
        }
        
        return status;
    }
};