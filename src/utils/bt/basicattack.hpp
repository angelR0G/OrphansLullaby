#pragma once

#include "../../Fmod/MasksPositions.hpp"

struct BTBasicAttack : BTNode {
    BTBasicAttack(){};

	BTNodeStatus run(EntityContext* ectx) noexcept final {
        auto status {BTNodeStatus::success};

        float attackDuration = ENEMY_BASIC_AT_DURATION;
        if(ectx->enemyType==2){
            attackDuration = ENEMY_THROWER_AT_DURATION;
        }
        if(ectx->enemyType==3){
            attackDuration = ENEMY_RUGBY_AT_DURATION;
        }

        //Atack when it reaches cooldown
        if(ectx->ai->attackCD <= attackDuration){
            status = BTNodeStatus::running;
        }
        else if(ectx->ai->attackCD <= ectx->atckstats->basic_cd){
            if(!ectx->atckstats->attacked){
                std::vector<float> atparams { 0, 10, 8, 3.0f, 10.0f, 3.0f, ectx->atckstats->basic_dmg, ectx->hp->infection, attackDuration};
                
                //Create attack
                EntityFactory* factory = EntityFactory::Instance();
                factory->createEnemyAttack(ectx->pos, atparams, ENEMY_ATTACK_MASK);

                //Play attack sounds
                ectx->sc->maskSounds[M_ZOMBIE_ATTACK] |= SOUND_PLAY;

                //Mark attack stat attacked
                ectx->atckstats->attacked = true;
            }

            status = BTNodeStatus::running;
        }else{
            //Reset duration
            ectx->ai->attackCD   = 0;   
        }

        return status;
    }
};