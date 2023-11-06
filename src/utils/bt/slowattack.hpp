#pragma once

//#include "node.hpp"
//#include "../../engine/entityFactory.hpp"
#include "../../Fmod/MasksPositions.hpp"

struct BTSlowAttack : BTNode {
    BTSlowAttack(){};

	BTNodeStatus run(EntityContext* ectx) noexcept final {
        auto status {BTNodeStatus::success};
        
        //Attack when it reaches cooldown
        if(ectx->ai->attackCD <= ENEMY_RUGBY_AT_SLOW_DURATION/2){
            ectx->ai->behaviour = SB::AFK;

            status = BTNodeStatus::running;
        }
        else if(ectx->ai->attackCD < ENEMY_RUGBY_AT_SLOW_COOLDOWN){
            //Orientate enemy to player
            constexpr auto PI {std::numbers::pi};
            auto distx { ectx->posPlayer->x - ectx->pos->x };
            auto distz { ectx->posPlayer->z - ectx->pos->z };
            auto torien { std::atan2(distx, distz) };

            if (torien < 0)    torien += 2*PI;
            ectx->pos->orientation = torien;
        }else{               
            std::vector<float> atparams { 0, 0, 6, 14.0f, 2.0f, 10.0f, RUGBY_ENEMY_DAMAGE_SLOW, ectx->hp->infection, ENEMY_RUGBY_AT_SLOW_DURATION};

            //Create attack
            EntityFactory* factory = EntityFactory::Instance();
            factory->createProjectileAttack(ectx->pos, atparams, PLAYER_COLLISION, PROJECTILE_SLOW_COLLISION);
            //Play spit sound
            ectx->sc->maskSounds[M_ZOMBIE_SPECIAL] |= SOUND_PLAY;

            //Active slow attack animation
            if(ectx->animC != nullptr){
                ectx->animC->rAnimation = static_cast<int>(RunningAnimationEnemy::ATTACK_ANIMATION1);
                ectx->animC->animations[ectx->animC->rAnimation].m_CurrentTime= 0.0;
            }

            ectx->ai->attackCD   = 0; 
        }

        return status;
    }
};