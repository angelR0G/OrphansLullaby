#pragma once

#include "sceneEntity.hpp"
#include "../particles/emitterTypes.hpp"
#include <memory>

struct ParticleEmitter;

namespace GE
{
    struct SceneParticleEmitter : public SceneEntity{
        SceneParticleEmitter(ParticleEmitterType);
        ~SceneParticleEmitter() = default;
        void updateEmitter(float, entityPos const&);
        void draw(const glm::mat4&, ShaderResource*);
        void setActive(bool) noexcept;
        void setActiveForTime(float) noexcept;
        void setInterpolationValue(float) noexcept;
        static void setPlayerReferencePosition(entityPos const&) noexcept;

        [[nodiscard]] bool getActive() const noexcept;

        private:
            std::unique_ptr<ParticleEmitter> emitter{nullptr};
    };
} //end namespace GE