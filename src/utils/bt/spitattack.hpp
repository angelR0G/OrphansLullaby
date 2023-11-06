#pragma once

//#include "node.hpp"
//#include "../../engine/entityFactory.hpp"
#include "../../Fmod/MasksPositions.hpp"

struct BTSpitAttack : BTNode {
    BTSpitAttack(){};

	BTNodeStatus run(EntityContext* ectx) noexcept final {
        auto status {BTNodeStatus::success};

        //Atack when it reaches cooldown
        if(ectx->ai->attackCD <= ENEMY_THROWER_AT_SPIT_DURATION/4){
            ectx->ai->behaviour = SB::AFK;

            status = BTNodeStatus::running;
        }
        else if(ectx->ai->attackCD < ENEMY_THROWER_AT_SPIT_COOLDOWN){
            //Orientate enemy to player
            constexpr auto PI {std::numbers::pi};
            auto distx { ectx->posPlayer->x - ectx->pos->x };
            auto distz { ectx->posPlayer->z - ectx->pos->z };
            auto torien { std::atan2(distx, distz) };

            if (torien < 0)    torien += 2*PI;
            ectx->pos->orientation = torien;
        }else{
            std::vector<float> atparams { 0, 20, 0, 3.0f, 2.0f, 5.0f, THROWER_ENEMY_DAMAGE_SPIT, ectx->hp->infection, ENEMY_THROWER_AT_SPIT_DURATION};
            //Create attack
            EntityFactory* factory = EntityFactory::Instance();
            auto attack {factory->createProjectileAttack(ectx->pos, atparams, PLAYER_COLLISION, PROJECTILE_SPIT_COLLISION)};

            // Orientate attack to player
            BasicComponent* bCmp {&GameManager::Instance()->EM.getComponent<BasicComponent>(*attack)};
            float x {ectx->posPlayer->x - bCmp->x};
            float z {ectx->posPlayer->z - bCmp->z};
            float y {(ectx->posPlayer->y + 8) - bCmp->y};
            // Distance to player(already calculated, can be optimized)
            float horizontalDistance = std::sqrt(x*x + z*z);
            bCmp->verticalOrientation = std::atan2(y, horizontalDistance);
            
            ectx->sc->maskSounds[M_ZOMBIE_ATTACK] |= SOUND_PLAY;
            //Play spit sound
            ectx->sc->maskSounds[M_ZOMBIE_SPECIAL] |= SOUND_PLAY;

            //Active spit attack animation
            if(ectx->animC != nullptr){
                ectx->animC->rAnimation = static_cast<int>(RunningAnimationEnemy::ATTACK_ANIMATION1);
                ectx->animC->animations[ectx->animC->rAnimation].m_CurrentTime= 0.0;
            }
                

            //Reset duration
            ectx->ai->attackCD = 0;
        }       
        
        return status;
    }

};