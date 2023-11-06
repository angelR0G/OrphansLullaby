#include "attack.hpp"
#include "../components/attack.hpp"
#include "../engine/entityFactory.hpp"

#define ROCKET_RADIUS 30.f

void AttackSystem::update(EntityMan& EM, float deltaTime) {
    using CList = MetaP::Typelist<AttackComponent>;
    using TList = MetaP::Typelist<>;

    EM.foreach<CList, TList>([&](Entity& e){
        // ir sumando currenttime + deltatime hasta pasar maxtime
        auto& at  = EM.getComponent<AttackComponent>(e);

        if(at.currentAttackTime>=at.maxDurationAttack){
            EntityFactory* factory = EntityFactory::Instance();

            // Check if it is a rocket
            CollisionComponent* cCmp = &EM.getComponent<CollisionComponent>(e);
            if(cCmp->collision.getUserData()->flags == PROJECTILE_ROCKET_COLLISION) {
                // Create an explosion at the rocket position
                BasicComponent* basicCmp = &EM.getComponent<BasicComponent>(e);
                std::vector<float> atparams {0, 0, 0, ROCKET_RADIUS, at.damage, at.infection};
                factory->createExplosion(e.getId(), basicCmp, atparams, at.damageTo);
                //Mark entity to destroy
                e.addTag<MarkToDestroyDelayTag>();
            }else{
                //Mark entity to destroy
                e.addTag<MarkToDestroyTag>();
            }

            
        }
        else{
            at.currentAttackTime += deltaTime;
        }
    });
}