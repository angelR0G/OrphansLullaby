#pragma once

#include <array>

#define SLOW_DURATION           5

// Effects value
#define SP_EFFECT_SPEED_INCREASE_VALUE		10.f
#define SP_EFFECT_DAMAGE_REDUCTION_VALUE	0.7f

// General effects macros
#define EFFECTS_INIT_HEALTH		2.f
#define MAX_EFFECTS				2
#define TOTAL_EFFECTS_COUNT		4
#define EFFECT_DISPLAY_TIME		3.f

enum class SpecialEffect : uint8_t {
	No_Effect	= 0,
	Inmunity	= 1,
	Tank		= 2,
	Speed		= 3,
	Steady_Aim	= 4,
};

using SpEffectsContainer 		= std::array<SpecialEffect, MAX_EFFECTS>;
using SpEffectsHealthContainer 	= std::array<float, MAX_EFFECTS>;

struct BasicComponent {
    float   x{}, prevx{}, 
            y{}, prevy{}, 
            z{}, prevz{};
    double  orientation{}, prevorientation {},
            verticalOrientation{},
            orientationAvoid{};
    float   slowEffect{};
    double  death_clock{};

    SpEffectsContainer 			spEffects{};
	SpEffectsHealthContainer	spEffectsHealth{};
	SpecialEffect				addSpEffect{};
	float						effectsHealthLost{};
};

bool hasSpecialEffect(SpEffectsContainer& effects, SpecialEffect desiredEffect);