#pragma once

#define EXPLOSIVE_ATTACK_DIST 16.0f

struct BTCheckExplosiveAttack: BTNode {
    BTCheckExplosiveAttack(){};

	BTNodeStatus run(EntityContext* ectx) noexcept final {
        auto status {BTNodeStatus::fail};

        // cambiar esto al fusionarlo con la ia actualizada
        auto distx { ectx->posPlayer->x - ectx->pos->x};
        auto distz { ectx->posPlayer->z - ectx->pos->z};
        auto distlin { std::sqrt(distx*distx + distz*distz) };

        if(distlin<=EXPLOSIVE_ATTACK_DIST){
            ectx->ai->behaviour = SB::AFK;

            //Play explosion attack animation
            ectx->animC->rAnimation = static_cast<int>(RunningAnimationEnemy::ATTACK_ANIMATION);
            
            //Orientate enemy to player
            constexpr auto PI {std::numbers::pi};
            auto distx { ectx->posPlayer->x - ectx->pos->x };
            auto distz { ectx->posPlayer->z - ectx->pos->z };
            auto torien { std::atan2(distx, distz) };

            if (torien < 0)    torien += 2*PI;
            ectx->pos->orientation = torien;
            
            status = BTNodeStatus::success;
        }
        else{
            ectx->ai->attackCD = 0;
        }

        return status;
    }
};