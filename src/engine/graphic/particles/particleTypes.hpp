#pragma once

#include "particle.hpp"
#include <string>
#include <array>

using entityPos = std::array<float, 3>;

struct ParticleType {
    ParticleType() = default;
    virtual ~ParticleType() = default;

    virtual void        updateParticle(Particle&, float)    noexcept    {};
    virtual Particle    newParticle(GE::Transform const&, entityPos)   noexcept    {return {};};

    const float generationTime{0.1};
    const float particlesLife{1};
    const std::string material{"media/enemies/materials/Escupitajo.mtl"};

    protected:
        ParticleType(float, float);
};

struct ExplosionParticle : public ParticleType {
    ExplosionParticle();

                    void        updateParticle(Particle&, float)    noexcept override;
    [[nodiscard]]   Particle    newParticle(GE::Transform const&, entityPos)   noexcept override;
};

struct InfectionExplosionParticle : public ParticleType {
    InfectionExplosionParticle();

                    void        updateParticle(Particle&, float)    noexcept override;
    [[nodiscard]]   Particle    newParticle(GE::Transform const&, entityPos)   noexcept override;
};

struct BloodParticle : public ParticleType {
    BloodParticle();

                    void        updateParticle(Particle&, float)    noexcept override;
    [[nodiscard]]   Particle    newParticle(GE::Transform const&, entityPos)   noexcept override;
};

struct DeathParticle : public ParticleType {
    DeathParticle();

                    void        updateParticle(Particle&, float)    noexcept override;
    [[nodiscard]]   Particle    newParticle(GE::Transform const&, entityPos)   noexcept override;
};
