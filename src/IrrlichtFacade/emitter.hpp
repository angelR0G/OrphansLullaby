#pragma once

#include "../engine/graphic/sceneTree/scenePartEmitter.hpp"
#include <array>

struct particleEmitter {

    void createParticleEmitter(ParticleEmitterType);
    void update(float);
    void setActive(bool) noexcept;
    void setActiveForTime(float) noexcept;
    void setPosition(float, float, float) noexcept;
    void setOffset(float, float, float) noexcept;
    void setRotation(float, float, float) noexcept;
    void setInterpolationValue(float) noexcept;
    static void setPlayerReferencePosition(entityPos const&) noexcept;

    [[nodiscard]] bool getActive() const noexcept;
    [[nodiscard]] entityPos getPosition() const noexcept;
    [[nodiscard]] GE::SceneParticleEmitter* getEmitter() const noexcept;

    private:
        GE::SceneParticleEmitter* emitter {nullptr};
        std::array<float, 3> offsetEmitter{0, 0, 0};
};