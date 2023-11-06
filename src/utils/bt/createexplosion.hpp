#pragma once

//#include "node.hpp"
//#include "../../engine/physics/physicsEngine.hpp"
//#include "../../engine/entityFactory.hpp"
//#include "../collisionTypes.hpp"
//#include <vector>

struct BTCreateExplosion : BTNode {
    BTCreateExplosion(){};

	BTNodeStatus run(EntityContext* ectx) noexcept final {
        std::vector<float> atparams { 0, 5, 0, 30.0f, EXPLOSIVE_ENEMY_DAMAGE, 0};

        //Create attack
        EntityFactory* factory = EntityFactory::Instance();
        Entity* explosion = factory->createExplosion(ectx->idEntity, ectx->pos, atparams, (PLAYER_COLLISION | ENEMY_COLLISION | DESTROYABLE_COLLISION));

        //Play broken barrel animation 
        if(ectx->animC != nullptr){
            ectx->animC->rAnimation = static_cast<int>(RunningAnimationBarrel::BREAK_ANIMATION);
            ectx->animC->prevAnimation = static_cast<int>(RunningAnimationBarrel::IDLE_ANIMATION_BREAK);
        }

        return BTNodeStatus::success;
    }
};