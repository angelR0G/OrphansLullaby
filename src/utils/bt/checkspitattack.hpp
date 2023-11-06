#pragma once

#define SPIT_ATTACK_DIST_MIN 60.0f
#define SPIT_ATTACK_DIST_MAX 100.0f

struct BTCheckSpitAttack: BTNode {
    BTCheckSpitAttack(){};

	BTNodeStatus run(EntityContext* ectx) noexcept final {
        auto status {BTNodeStatus::fail};

        // cambiar esto al fusionarlo con la ia actualizada
        auto distx { ectx->posPlayer->x - ectx->pos->x};
        auto distz { ectx->posPlayer->z - ectx->pos->z};
        auto distlin { std::sqrt(distx*distx + distz*distz) };

        if(distlin>=SPIT_ATTACK_DIST_MIN && distlin<=SPIT_ATTACK_DIST_MAX){
            ectx->ai->behaviour = SB::AFK;
            
            //Orientate enemy to player
            constexpr auto PI {std::numbers::pi};
            auto distx { ectx->posPlayer->x - ectx->pos->x };
            auto distz { ectx->posPlayer->z - ectx->pos->z };
            auto torien { std::atan2(distx, distz) };

            if (torien < 0)    torien += 2*PI;
            ectx->pos->orientation = torien;

            ectx->ai->attackCD = 0;
            ectx->atckstats->attacked = false;

            //Active spit attack animation
            if(ectx->animC != nullptr){
                ectx->animC->rAnimation = static_cast<int>(RunningAnimationEnemy::ATTACK_ANIMATION1);
                ectx->animC->animations[ectx->animC->rAnimation].m_CurrentTime= 0.0;
            }
            
            status = BTNodeStatus::success;
        }

        return status;
    }
};