#pragma once

#include "../engine/types.hpp"
#include "../manager/playerStats.hpp"

struct BasicSystem {
    void update(EntityMan& EM, float, playerStats& pStats);

    private:
        void enableSpecialEffect(SpecialEffect, Entity&, EntityMan&);
        void disableSpecialEffect(SpecialEffect, Entity&, EntityMan&);
        void removeEffects(SpEffectsContainer&, SpEffectsHealthContainer&, Entity&, EntityMan&);
        size_t getSuitablePosition(SpEffectsContainer&, SpEffectsHealthContainer&);
        size_t addNewEffect(EntityMan&, Entity&);
        SpecialEffect getValidEffect(SpEffectsContainer&, SpecialEffect);
};