#pragma once

#include "../components/basic.hpp"

struct playerStats {
    float   health{},
            infection{};
    SpEffectsContainer              specialEffect{};
    SpEffectsHealthContainer        specialEffectHealth{};
    std::array<float, MAX_EFFECTS>  effectsDisplayTime{};
};