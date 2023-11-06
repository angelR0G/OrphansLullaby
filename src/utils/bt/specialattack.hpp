#pragma once

#include "../../Fmod/MasksPositions.hpp"

struct BTSpecialAttack : BTNode {
    BTSpecialAttack(){};

	BTNodeStatus run(EntityContext* ectx) noexcept final {
        auto status {BTNodeStatus::success};

        // get attack duration of enemy
        float attackStopDuration    = ENEMY_THROWER_AT_SPIT_DURATION/4,
              attackDuration        = ENEMY_THROWER_AT_SPIT_DURATION,
              startAnimationTime    = 0.15,
              animationDuration     = ectx->atckstats->special_cd;
        if(ectx->enemyType==3){
            attackDuration      = ENEMY_RUGBY_AT_SLOW_DURATION;
            attackStopDuration  = ENEMY_RUGBY_AT_SLOW_DURATION/2;
            startAnimationTime  = 0.95;
            animationDuration   = 1.75;
        }

        //Atack when it reaches cooldown
        if(ectx->ai->attackCD <= startAnimationTime){
            status = BTNodeStatus::running;
        }
        else if(ectx->ai->attackCD < animationDuration){
            status = BTNodeStatus::running;

            if(!ectx->atckstats->attacked){
                // Create special attack parameters checking the type of enemy, default 2: thrower
                uint8_t collisionType = PROJECTILE_SPIT_COLLISION;
                std::vector<float> atparams { 0, 20, 0, 3.0f, 2.0f, 5.0f, ectx->atckstats->special_dmg, ectx->hp->infection, attackDuration};

                if(ectx->enemyType==3){
                    collisionType = PROJECTILE_SLOW_COLLISION;
                    atparams = { 0, 0, 6, 14.0f, 2.0f, 10.0f, ectx->atckstats->special_dmg, ectx->hp->infection, attackDuration};
                }

                //Create attack
                EntityFactory* factory = EntityFactory::Instance();
                auto attack {factory->createProjectileAttack(ectx->pos, atparams, PLAYER_COLLISION, collisionType)};

                // Orientate attack to player if its enemy thrower
                if(ectx->enemyType==2){
                    BasicComponent* bCmp {&GameManager::Instance()->EM.getComponent<BasicComponent>(*attack)};
                    float x {ectx->posPlayer->x - bCmp->x};
                    float z {ectx->posPlayer->z - bCmp->z};
                    float y {(ectx->posPlayer->y + 8) - bCmp->y};

                    // Distance to player(already calculated, can be optimized)
                    float horizontalDistance = std::sqrt(x*x + z*z);
                    bCmp->verticalOrientation = std::atan2(y, horizontalDistance);
                }

                // Play special attack sound
                ectx->sc->maskSounds[M_ZOMBIE_SPECIAL] |= SOUND_PLAY;

                ectx->atckstats->attacked = true;
            }
        }else{
            //Reset duration
            ectx->ai->attackCD = 0;
        }

        return status;
    }
};