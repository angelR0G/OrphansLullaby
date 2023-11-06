#include "scenePartEmitter.hpp"
#include "../particles/particleEmitter.hpp"

GE::SceneParticleEmitter::SceneParticleEmitter(ParticleEmitterType t) : GE::SceneEntity () {
    emitter = std::unique_ptr<ParticleEmitter>(new ParticleEmitter(t));
}

void GE::SceneParticleEmitter::updateEmitter(float dt, entityPos const& ePos) {
    emitter->setEmitterPosition(ePos);
    emitter->update(dt);
}

void GE::SceneParticleEmitter::draw(const glm::mat4& model, ShaderResource* shader) {
    emitter->draw(model, shader);
}

void GE::SceneParticleEmitter::setActive(bool a) noexcept{
    emitter->setActive(a);
}

void GE::SceneParticleEmitter::setActiveForTime(float time) noexcept {
    emitter->setActiveForTime(time);
}

bool GE::SceneParticleEmitter::getActive() const noexcept {
    return emitter->getActive();
}

void GE::SceneParticleEmitter::setInterpolationValue(float val) noexcept {
    emitter->interpolationValue = val;
}

void GE::SceneParticleEmitter::setPlayerReferencePosition(entityPos const& pos) noexcept {
    ParticleEmitter::setPlayerReferencePosition(pos);
}