#include "health.hpp"

#include "../manager/eventmanager.hpp"
#include "../manager/gamemanager.hpp"
#include "../engine/entityFactory.hpp"
#include "../engine/types.hpp"
#include "../Fmod/MasksPositions.hpp"

uint8_t HealthSystem::healthCurrentId = 1;

uint8_t HealthSystem::checkEnemyType(Entity& e){
    uint8_t type = 0;
    if(e.template hasTag<EnemyNormalTag>()){
        type = ENEMY_NORMAL;
    }else if(e.template hasTag<EnemyExplosiveTag>()){
        type = ENEMY_EXPLOSIVE;
    }else if(e.template hasTag<EnemyThrowerTag>()){
        type = ENEMY_THROWER;
    }else if(e.template hasTag<EnemyRugbyTag>()){
        type = ENEMY_RUGBY;
    }
    return type;
}

uint8_t HealthSystem::checkEnemyHeadShot(Entity& e){
    uint8_t type = 0;
    
    if(e.hasTag<HeadShotMarkTag>()) type = 1;
    
    return type;
}


void HealthSystem::update(EntityMan& EM, float deltaTime, playerStats& pStats) {
    using CList = MetaP::Typelist<HealthComponent>;
    using TList = MetaP::Typelist<>;
    EM.foreach<CList, TList>([&](EntityMan::Entity& e){
        HealthComponent* hCmp       = &EM.getComponent<HealthComponent>(e);

        // Set an ID for the health component
        if(hCmp->healthId == 0) {
            hCmp->healthId = healthCurrentId;

            // Modify next ID
            ++healthCurrentId;
            if(healthCurrentId == 255) healthCurrentId = 1;
        }

        if(e.hasTag<PlayerTag>()) {
            // Player health control
            if(hCmp->infection > MAX_PLAYER_INFECTION)      hCmp->infection = MAX_PLAYER_INFECTION;
            if(hCmp->infection < 0)                         hCmp->infection = 0;

            hCmp->health += (5-hCmp->infection) * deltaTime;

            // Clamp health
            if(hCmp->health <= 0.0f)                                hCmp->health = 0.0f;
            else if(hCmp->health > 100 - (int)hCmp->infection*10)   hCmp->health = 100 - (int)hCmp->infection*10;

            pStats.health = hCmp->health;
            pStats.infection = hCmp->infection;

            //Play heart sound
            SoundComponent* sC    = &EM.getComponent<SoundComponent>(e);
            if(pStats.health<=25 && !hCmp->criticHealth){
                sC->maskSounds[M_LOW_HEALTH] |= SOUND_PLAY;
                hCmp->criticHealth = true;
            }
            if(pStats.health>35 && hCmp->criticHealth){
                sC->maskSounds[M_LOW_HEALTH] |= SOUND_STOP;
                hCmp->criticHealth = false;
            }
            
        }
        else if(hCmp->health <= 0.0f){
            if(e.hasTag<ReparableTag>()) {
                // Check if is a defense objective
                if((e.hasTag<GoalDefendTag>() || e.hasTag<MovableObjectiveTag>()) && !e.hasTag<BrokenObjectTag>()) {
                    GameManager* gm {GameManager::Instance()};
                    
                    if(gm->getGoals()->getType() == GoalTypes::ContinuousProgress) {
                        // Get goal progress
                        auto progress {gm->getGoals()->getCurrentProgress()};
                        float objective {std::get<float>(progress.second)};

                        // Reduce progress a little
                        GameManager::Instance()->completeGoal(-objective/10.f);
                    }
                    using PlayerComp = MetaP::Typelist<>;
                    using PlayerT = MetaP::Typelist<PlayerTag>;
                    Entity* player = EM.search<PlayerComp, PlayerT>()[0];
                    SoundComponent* sC    = &EM.getComponent<SoundComponent>(*player);
                    sC->maskSounds[M_DIALOGUES] |= SOUND_PLAY;
                    sC->paramValues.insert_or_assign("radioDialogue", RADIO_DIALOGUE_PLAYING);
                    sC->paramValues.insert_or_assign("line", RADIO_ISSUE);
                    e.addTag<BrokenObjectTag>();
                }

            }
            else if(e.hasTag<EnemyTag>()) {
                // Delete dead enemies
                EntityFactory* factory = EntityFactory::Instance();
                EventManager* eventMan = EventManager::Instance();
                std::shared_ptr<Event> event ( new Event(EVENT_ENEMYDEAD) );
                event.get()->dataNum    = checkEnemyType(e);
                event.get()->dataNum2   = checkEnemyHeadShot(e);
                
                eventMan->scheduleEvent(event);

                //Play death animation
                deathAnimation(e, EM);
                
                //Mark entity to destroy
                e.addTag<MarkToDestroyTag>();
                factory->checkDeleteEnemy(e);
                //factory->deleteEnemyNormal(e);
                GameManager* gm = GameManager::Instance();
                gm->decreaseEnemy();
            }
            else if(e.hasTag<ExplosiveBarrelTag>() || e.hasTag<TargetBarrelTag>()){
                if(hCmp->health < 0.0f)     hCmp->health = 0.f;
            }
            /*
            * Special entities
            */
            else if(e.hasTag<ElectricPanelTag>()) {
                auto gm {GameManager::Instance()};
                // Add progress to goal
                gm->completeGoal(1);

                EventManager* eventMan = EventManager::Instance();
                std::shared_ptr<Event> event ( new Event(EVENT_BREAK_ELECTRIC_PANEL_GOAL) );
                event->dataNum = e.getId();
                SoundComponent* sC    = &EM.getComponent<SoundComponent>(e);
                sC->maskSounds[M_ELECTRIC_PANEL] |= SOUND_STOP;

                eventMan->scheduleEvent(event);
            }
            else if(e.hasTag<MallNestTag>()) {
                auto gm {GameManager::Instance()};
                // Add progress to goal
                gm->completeGoal(1);

                // Spawn a special enemy
                EventManager* eventMan = EventManager::Instance();
                std::shared_ptr<Event> event ( new Event(EVENT_SPAWN_NEW_ENEMY) );
                event->dataNum  = 1;
                event->dataNum2 = 3;

                eventMan->scheduleEvent(event);

                // Destroy entity
                gm->explodeGoal(e.getId(), ParticleEmitterType::InfectionExplosionParticle);
            }
            if(e.hasTag<M1K3Tag>()) {
                //Play sad sound
                SoundComponent* sC    = &EM.getComponent<SoundComponent>(e);
                if(sC->paramValues.find("M1K3estado") != sC->paramValues.end()){
                    if(sC->paramValues.find("M1K3estado")->second !=2){
                        sC->paramValues.insert_or_assign("M1K3estado", 2);
                        sC->maskSounds[M_M1K3_FLY]          |=  SOUND_UPDATE_PARAM;
                    }
                }
            }
        }
    });

}

void HealthSystem::deathAnimation(Entity& e, EntityMan& EM){
    //Activate death animation
    if(e.hasComponent<AnimationComponent>()){
        AnimationComponent *acmp = &EM.getComponent<AnimationComponent>(e);
        acmp->rAnimation = static_cast<int>(RunningAnimationEnemy::DEATH_ANIMATION);
        acmp->prevAnimation = static_cast<int>(RunningAnimationEnemy::DEATH_ANIMATION);
    }

    //Crate particles for death animation
    if(e.hasComponent<RenderComponent>()){
        RenderComponent* rcmp = &EM.getComponent<RenderComponent>(e);
        //Create explosive emitter
        rcmp->emitter.createParticleEmitter(ParticleEmitterType::DeathParticle);
        rcmp->emitter.setActiveForTime(3.0);
    }
}