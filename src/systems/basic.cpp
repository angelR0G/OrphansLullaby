#include "basic.hpp"
#include "../components/basic.hpp"
#include "../Fmod/MasksPositions.hpp"
void playEffectSound(int effect, Entity* e, EntityMan* EM);
void BasicSystem::update(EntityMan& EM, float deltaTime, playerStats& pStats) {
    using CList = MetaP::Typelist<BasicComponent>;
    using TList = MetaP::Typelist<PlayerTag>;
    EM.foreach<CList, TList>([&](Entity& e){
        BasicComponent& bCmp = EM.getComponent<BasicComponent>(e);
        auto& healthLost {bCmp.effectsHealthLost};

        if(healthLost > 0.01) {
            for(auto& effectHealth : bCmp.spEffectsHealth) {
                effectHealth -= healthLost;
            }

            healthLost = 0;

            // Remove effects which health is below 0
            removeEffects(bCmp.spEffects, bCmp.spEffectsHealth, e, EM);
        }

        // Add a new effect
        if(bCmp.addSpEffect != SpecialEffect::No_Effect) {
            // Check if new effect is valid
            bCmp.addSpEffect = getValidEffect(bCmp.spEffects, bCmp.addSpEffect);
            playEffectSound(static_cast<int>(bCmp.addSpEffect), &e, &EM);


            // Enables effect and add it to the effects container
            enableSpecialEffect(bCmp.addSpEffect, e, EM);
            
            auto effectIndex = addNewEffect(EM, e);
            pStats.effectsDisplayTime[effectIndex] = EFFECT_DISPLAY_TIME;
        }

        // Save data for HUD representation
        pStats.specialEffect        = bCmp.spEffects;
        pStats.specialEffectHealth  = bCmp.spEffectsHealth;

        // Update display time
        for(auto& dispTime : pStats.effectsDisplayTime) {
            if(dispTime > 0) dispTime -= deltaTime;
        }
    });
}

void BasicSystem::enableSpecialEffect(SpecialEffect effect, Entity& ent, EntityMan& EM) {
    if(effect == SpecialEffect::Speed) {
        MovementComponent& mCmp = EM.getComponent<MovementComponent>(ent);

        mCmp.maxVelocity    += SP_EFFECT_SPEED_INCREASE_VALUE;
        mCmp.maxRunVelocity += SP_EFFECT_SPEED_INCREASE_VALUE;
    }
    else if(effect == SpecialEffect::Steady_Aim) {
        WeaponComponent& wCmp   = EM.getComponent<WeaponComponent>(ent);

        wCmp.firstWeapon->recoilForce   /= 2.0f;
        wCmp.secondWeapon->recoilForce  /= 2.0f;
    }
}

void BasicSystem::disableSpecialEffect(SpecialEffect effect, Entity& ent, EntityMan& EM) {
    if(effect == SpecialEffect::Speed) {
        MovementComponent& mCmp = EM.getComponent<MovementComponent>(ent);

        mCmp.maxVelocity    -= SP_EFFECT_SPEED_INCREASE_VALUE;
        mCmp.maxRunVelocity -= SP_EFFECT_SPEED_INCREASE_VALUE;
    }
    else if(effect == SpecialEffect::Steady_Aim) {
        WeaponComponent& wCmp   = EM.getComponent<WeaponComponent>(ent);

        wCmp.firstWeapon->recoilForce   *= 2.0f;
        wCmp.secondWeapon->recoilForce  *= 2.0f;
    }
}

size_t BasicSystem::addNewEffect(EntityMan& EM, Entity& ent) {
    BasicComponent& bCmp    = EM.getComponent<BasicComponent>(ent);

    auto newEffectPos       = getSuitablePosition(bCmp.spEffects, bCmp.spEffectsHealth);

    disableSpecialEffect(bCmp.spEffects[newEffectPos], ent, EM);

    bCmp.spEffects[newEffectPos]        = bCmp.addSpEffect;
    bCmp.spEffectsHealth[newEffectPos]  = EFFECTS_INIT_HEALTH;

    bCmp.addSpEffect        = SpecialEffect::No_Effect;

    return newEffectPos;
}

SpecialEffect BasicSystem::getValidEffect(SpEffectsContainer& container, SpecialEffect defaultEffect) {
    bool repeatedEffect{false};

    do {
        repeatedEffect = false;

        for(auto effect : container) {
            if(effect == defaultEffect) {
                repeatedEffect = true;

                // Change effect
                int eIndex = (int)defaultEffect - 1;
                if(eIndex <= 0)  eIndex = (int)SpecialEffect::Steady_Aim;
                
                defaultEffect = SpecialEffect(eIndex);
            }
        }
    } while(repeatedEffect);

    return defaultEffect;
}

void BasicSystem::removeEffects(SpEffectsContainer& container, SpEffectsHealthContainer& health, Entity& ent, EntityMan& EM) {
    for(size_t i{0}; i < health.size(); ++i) {
        if(health[i] < 0.01) {
            disableSpecialEffect(container[i], ent, EM);
            container[i] = SpecialEffect::No_Effect;
        }
    }
}

size_t BasicSystem::getSuitablePosition(SpEffectsContainer& container, SpEffectsHealthContainer& health) {
    size_t lowestHealthPos {0};

    for(size_t i{0}; i<container.size(); ++i) {
        // Check if there is a free space
        if(container[i] == SpecialEffect::No_Effect)
            return i;

        // Check if has lower health
        if(health[i] < health[lowestHealthPos]) {
            lowestHealthPos = i;
        }
    }

    return lowestHealthPos;
}

bool hasSpecialEffect(SpEffectsContainer& effects, SpecialEffect desiredEffect) {
	for(auto& eValue : effects) if(eValue == desiredEffect) return true;
	return false;
}

void playEffectSound(int effect, Entity* e, EntityMan* EM){
    SoundComponent* soundCmp = &EM->getComponent<SoundComponent>(*e);
    soundCmp->paramValues.insert_or_assign("Mejora", effect);
    soundCmp->maskSounds[M_BOOST_MACHINE] |= SOUND_UPDATE_PARAM;
    soundCmp->maskSounds[M_BOOST_MACHINE] |= SOUND_PLAY;
    soundCmp->maskSounds[M_GET_OBJECT] |= SOUND_PLAY;
    
}