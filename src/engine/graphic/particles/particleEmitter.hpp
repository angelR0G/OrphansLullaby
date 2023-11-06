#pragma once

#include "particleTypes.hpp"
#include "emitterTypes.hpp"
#include "../resource/materialresource.hpp"

#include <glad/glad.h>
#include <vector>
#include <memory>

#define MAX_GENERATION_UPDATE 40

struct ResourceManager;
struct ShaderResource;

struct ParticleEmitter {
    ParticleEmitter(ParticleEmitterType t);

    ~ParticleEmitter();

    void update(float dt);
    void draw(glm::mat4 model, ShaderResource* shader);

    void setActive(bool) noexcept;
    void setActiveForTime(float) noexcept;
    void setEmitterPosition(entityPos const&) noexcept;
    static void setPlayerReferencePosition(entityPos const&) noexcept;
    static void setGenerationFactor(float) noexcept;
    static void setInterpolation(bool) noexcept;

    [[nodiscard]] bool getActive() const noexcept;


    GE::Transform   emitterTransform{};
    float           lastGeneration{};
    float           interpolationValue{};

    private:
        bool                            active{true};
        bool                            activeForTime{false};
        float                           activeRemainingTime{-1};
        std::unique_ptr<ParticleType>   type{nullptr};
        std::vector<Particle>           particles{};
        size_t                          particlesCount{0};
        inline static float             generationFactor = 1.0;
        inline static bool              interpolateParticles{true};
        inline static entityPos         playerPos{};

        // Render variables
        inline static ResourceManager* resourceMan{nullptr};
        MaterialResource* material  {nullptr};
        unsigned int VAO, VBO;
        inline static GLint uModel {-1},
                            uColor {-1};

        void killParticle(std::vector<Particle>::iterator p) noexcept;
        void updateParticleOrientation(Particle&) noexcept;
        glm::mat4 getInterpolatedTransform(Particle&) const noexcept;
};