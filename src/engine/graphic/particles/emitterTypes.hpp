#pragma once

#include <array>
using entityPos = std::array<float, 3>;

enum class ParticleEmitterType {
    ExplosionParticle,
    InfectionExplosionParticle,
    BloodParticle,
    DeathParticle,
};