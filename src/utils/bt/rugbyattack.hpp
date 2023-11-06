#pragma once

//#include "node.hpp"
//#include "../../engine/entityFactory.hpp"
#include "../../Fmod/MasksPositions.hpp"

struct BTRugbyAttack : BTNode {
    BTRugbyAttack(){};

	BTNodeStatus run(EntityContext* ectx) noexcept final {
        auto status {BTNodeStatus::success};
        
        //Attack when it reaches cooldown
        if(ectx->ai->attackCD <= ENEMY_RUGBY_AT_DURATION){
            status = BTNodeStatus::running;
        }
        else if(ectx->ai->attackCD < ENEMY_RUGBY_AT_COOLDOWN){
            //Orientate enemy to player
            constexpr auto PI {std::numbers::pi};
            auto distx { ectx->posPlayer->x - ectx->pos->x };
            auto distz { ectx->posPlayer->z - ectx->pos->z };
            auto torien { std::atan2(distx, distz) };

            if (torien < 0)    torien += 2*PI;
            ectx->pos->orientation = torien;
        }else{
            std::vector<float> atparams { 0, 10, 8, 3.0f, 3.0f, 3.0f, RUGBY_ENEMY_DAMAGE, ectx->hp->infection, ENEMY_RUGBY_AT_DURATION};

            //Create attack
            EntityFactory* factory = EntityFactory::Instance();
            factory->createEnemyAttack(ectx->pos, atparams, ENEMY_ATTACK_MASK);
            //Play attack sounds
            ectx->sc->maskSounds[M_ZOMBIE_ATTACK] |= SOUND_PLAY;
            
            //Active normal attack animation
            if(ectx->animC != nullptr){
                ectx->animC->rAnimation = static_cast<int>(RunningAnimationEnemy::ATTACK_ANIMATION);
                ectx->animC->animations[ectx->animC->rAnimation].m_CurrentTime= 0.0;
            }

            ectx->sc->maskSounds[M_ZOMBIE_SCREAM_ATTACK] |= SOUND_PLAY;  
            //Reset duration
            ectx->ai->attackCD   = 0;
        }
        
        return status;   
    }
};