#include "types.hpp"
//#include "entitymanager.hpp"
#include "physics/physicsEngine.hpp"
#ifdef WINDOWS
    #include "../Fmod/SoundEngineWin.hpp"
#else
    #include "../Fmod/SoundEngine.hpp"
#endif
#include "graphic/engine.hpp"

#define DEATH_DELAY_TIME 6

//Search marked entities
template<typename CompList, typename TagList, std::uint32_t Capacity>
void EntityManager<CompList, TagList, Capacity>::searchMarkedEntities(PhysicsEngine* phy_engine, SoundEngine* sE, GraphicEngine* graphicEng, const double dt){
    for(auto& e : entities_){
        if(e.template hasComponent<BasicComponent>()){
            BasicComponent* basicCmp = &getComponent<BasicComponent>(e);

            //The game closed and dont need death delay for entities
            if(dt<0) e.template removeTag<MarkToDestroyDelayTag>();
            
            //Update time for enemy destruction
            if(e.template hasTag<MarkToDestroyDelayTag>()){
                if(basicCmp->death_clock < DEATH_DELAY_TIME)
                    basicCmp->death_clock += dt;
                else{
                    basicCmp->death_clock = 0;
                    e.template removeTag<MarkToDestroyDelayTag>();
                    e.template addTag<MarkToDestroyTag>();
                }
            }
        }
        //Check tags and delete all components
        if(e.template hasTag<MarkToDestroyTag>() || e.template hasTag<MarkToDestroyDelayTag>()){
            if(e.template hasComponent<BasicComponent>()){
                BasicComponent* basicComp = &getComponent<BasicComponent>(e);
                if(!e.template hasTag<MarkToDestroyDelayTag>()){
                    removeComponent<BasicComponent>(e);
                }else{
                    basicComp->prevx = basicComp->x;
                    basicComp->prevy = basicComp->y;
                    basicComp->prevz = basicComp->z;
                    basicComp->prevorientation = basicComp->orientation;
                }
            }
            

            if(e.template hasComponent<MovementComponent>()){
                removeComponent<MovementComponent>(e);
            }
            if(e.template hasComponent<RenderComponent>()){
                RenderComponent *rcmp = &getComponent<RenderComponent>(e);
                if(!e.template hasTag<MarkToDestroyDelayTag>()){
                    if(rcmp->node.getMesh() != nullptr)
                        graphicEng->removeSceneMesh(rcmp->node.getMesh(), false);
                    if(rcmp->emitter.getEmitter() != nullptr)
                        graphicEng->removeParticleEmitter(rcmp->emitter.getEmitter(), false);
                    removeComponent<RenderComponent>(e);
                }else if(!e.template hasComponent<AnimationComponent>()){
                    rcmp->node.setVisible(false);
                }
            }

            if(e.template hasComponent<AnimationComponent>()){
                AnimationComponent *acmp = &getComponent<AnimationComponent>(e);
                if(!e.template hasTag<MarkToDestroyDelayTag>()){
                    acmp->animations.clear();
                    removeComponent<AnimationComponent>(e);
                }
            }
            
            if(e.template hasComponent<InputComponent>()){
                removeComponent<InputComponent>(e);
            }
            if(e.template hasComponent<CameraComponent>()){
                removeComponent<CameraComponent>(e);
            }
            if(e.template hasComponent<AIComponent>()){
                AIComponent &aicmp = getComponent<AIComponent>(e);
                //delete aicmp.behaviourTree;
                aicmp.behaviourTree = nullptr;
                removeComponent<AIComponent>(e);
            }
            if(e.template hasComponent<WeaponComponent>()){
                removeComponent<WeaponComponent>(e);
            }
            if(e.template hasComponent<CollisionComponent>()){
                CollisionComponent &ccmp = getComponent<CollisionComponent>(e);
                phy_engine->deleteCollisionObject(&ccmp.collision);
                removeComponent<CollisionComponent>(e);
            }
            if(e.template hasComponent<SoundComponent>()){
                SoundComponent &sc = getComponent<SoundComponent>(e);
                for(unsigned int i=0; i<sc.idInstance.size(); i++){
                    sE->stopSound(sc.idInstance[i]);
                    sE->releaseSound(sc.idInstance[i]);
                }
                removeComponent<SoundComponent>(e);
            }
            if(e.template hasComponent<HealthComponent>()){
                removeComponent<HealthComponent>(e);
            }
            if(e.template hasComponent<NavigationComponent>()){
                NavigationComponent &navcmp = getComponent<NavigationComponent>(e);
                navcmp.path.clear();
                removeComponent<NavigationComponent>(e);
            }
            if(e.template hasComponent<AttackComponent>()){
                removeComponent<AttackComponent>(e);
            }
            if(e.template hasComponent<AttackStatsComponent>()) {
                removeComponent<AttackStatsComponent>(e);
            }
            if(e.template hasComponent<TriggerComponent>()){
                TriggerComponent &tcmp = getComponent<TriggerComponent>(e);
                phy_engine->deleteCollisionObject(&tcmp.collision);
                removeComponent<TriggerComponent>(e);
            }
            if(!e.template hasTag<MarkToDestroyDelayTag>()){
                e.template removeTag<MarkToDestroyTag>();
                deleteEntity(e.getId());
            }
        }
    }
}

template void EntityMan::searchMarkedEntities(PhysicsEngine* phy_engine, SoundEngine* sE, GraphicEngine* graphicEng, const double dt);