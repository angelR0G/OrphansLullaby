#pragma once

#include "../../Fmod/MasksPositions.hpp"

#define ATTACK_RADIUS 14

struct BTCheckAttackDist: BTNode {
    BTCheckAttackDist(){};

	BTNodeStatus run(EntityContext* ectx) noexcept final {
        auto status {BTNodeStatus::fail};

        // cambiar esto al fusionarlo con la ia actualizada
        auto distx { ectx->posPlayer->x - ectx->pos->x};
        auto distz { ectx->posPlayer->z - ectx->pos->z};
        auto distlin { std::sqrt(distx*distx + distz*distz) };

        if(distlin<=ATTACK_RADIUS){
            ectx->ai->behaviour = SB::AFK;
            
            //Orientate enemy to player
            constexpr auto PI {std::numbers::pi};
            auto distx { ectx->posPlayer->x - ectx->pos->x };
            auto distz { ectx->posPlayer->z - ectx->pos->z };
            auto torien { std::atan2(distx, distz) };

            if (torien < 0)    torien += 2*PI;
            ectx->pos->orientation = torien;

            //prueba para ataque basico bien
            ectx->ai->attackCD = 0;
            ectx->atckstats->attacked = false;

            //Active normal attack animation (antes estaba en basicattack)
            if(ectx->animC != nullptr){
                ectx->animC->rAnimation = static_cast<int>(RunningAnimationEnemy::ATTACK_ANIMATION);
                ectx->animC->animations[ectx->animC->rAnimation].m_CurrentTime= 0.0;
            }
            ectx->sc->maskSounds[M_ZOMBIE_SCREAM_ATTACK] |= SOUND_PLAY;
            status = BTNodeStatus::success;
        }

        return status;
    }
};