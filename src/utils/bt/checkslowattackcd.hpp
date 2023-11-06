#pragma once

#define SLOW_ATTACK_DIST 40.0f

struct BTCheckSlowAttackCD: BTNode {
    BTCheckSlowAttackCD(){};

	BTNodeStatus run(EntityContext* ectx) noexcept final {
        auto status {BTNodeStatus::fail};

        // cambiar esto al fusionarlo con la ia actualizada
        auto distx { ectx->posPlayer->x - ectx->pos->x};
        auto distz { ectx->posPlayer->z - ectx->pos->z};
        auto distlin { std::sqrt(distx*distx + distz*distz) };

        if(distlin<=SLOW_ATTACK_DIST){
            if(ectx->ai->attackCD>=ENEMY_RUGBY_AT_SLOW_COOLDOWN){
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

                //Active splow attack animation
                if(ectx->animC != nullptr){
                    ectx->animC->rAnimation = static_cast<int>(RunningAnimationEnemy::ATTACK_ANIMATION1);
                }

                status = BTNodeStatus::success;
            }
        }

        return status;
    }
};