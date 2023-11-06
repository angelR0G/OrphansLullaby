#include "trigger.hpp"

#include "../manager/gamemanager.hpp"
#include "../engine/entityFactory.hpp"
#include "../engine/physics/physicsEngine.hpp"
#include "../engine/graphic/engine.hpp"

#include "../utils/WeaponTemplates.hpp"

#include "Fmod/MasksPositions.hpp"
#include <string>


void TriggerSystem::update(EntityMan& EM, float deltaTime) {
    using CList = MetaP::Typelist<TriggerComponent>;
    using TList = MetaP::Typelist<>;
    bool    anyCollision    {false};

    EM.foreach<CList, TList>([&](Entity& e){
        TriggerComponent* triggerCmp    = &EM.getComponent<TriggerComponent>(e);
        BasicComponent* basicCmp        = &EM.getComponent<BasicComponent>(e);

        // If trigger is disabled, return
        if(!triggerCmp->bActive) return;

        // Update trigger transform
        triggerCmp->collision.setTransform(basicCmp->x, basicCmp->y, basicCmp->z, basicCmp->orientation, basicCmp->verticalOrientation);
        
        // if(e.template hasTag<BulletMachineTag>()){
        //     SoundComponent* soundCmp = &EM.getComponent<SoundComponent>(e);
        //     for(size_t i{}; i<soundCmp->maskSounds.size(); ++i){
        //         soundCmp->maskSounds[M_AMMO_MACHINE_GREETINGS] &= ~SOUND_PLAY;
        //     }
        // }
        // else if(e.template hasTag<InjectionMachineTag>()){
        //     SoundComponent* soundCmp = &EM.getComponent<SoundComponent>(e);
        //     for(size_t i{}; i<soundCmp->maskSounds.size(); ++i){
        //         soundCmp->maskSounds[M_HEAL_MACHINE_GREETINGS] &= ~SOUND_PLAY;
        //     }
        // }

        //Check events when player enters trigger
        // if(triggerCmp->playerEnter) {
        //     if(e.template hasTag<BulletMachineTag>()){
        //         SoundComponent* soundCmp = &EM.getComponent<SoundComponent>(e);
        //         soundCmp->maskSounds[M_AMMO_MACHINE_GREETINGS] |= SOUND_PLAY;
        //     }
        //     else if(e.template hasTag<InjectionMachineTag>()){
        //         SoundComponent* soundCmp = &EM.getComponent<SoundComponent>(e);
        //         soundCmp->maskSounds[M_HEAL_MACHINE_GREETINGS] |= SOUND_PLAY;
        //     }
        // }

        // Check if player is inside
        if(triggerCmp->playerInside) {
            if(e.hasTag<GoalAreaTag>()) {
                // Inside of a goal trigger
                if(GameManager::Instance()->completeGoal(deltaTime))
                    e.addTag<MarkToDestroyTag>();
            }
        }

        if(triggerCmp->playerInside) anyCollision = true;
        
        // Update trigger values
        if(triggerCmp->prevPlayerInside && !triggerCmp->playerInside)   triggerCmp->playerExit  = true;
        else                                                            triggerCmp->playerExit  = false;

        if(!triggerCmp->prevPlayerInside && triggerCmp->playerInside)   triggerCmp->playerEnter = true;
        else                                                            triggerCmp->playerEnter = false;

        triggerCmp->inside              = 0;
        triggerCmp->prevPlayerInside    = triggerCmp->playerInside;
        triggerCmp->playerInside        = false;
    });

    if(!anyCollision) {
        // If player does not collide with any triggers, deactivate messages
        GameManager::Instance()->activeText(false);
    }
}

void TriggerSystem::initPhysics(PhysicsEngine *phy)
{
    phy_engine = phy;
}
