#include "emitter.hpp"

#include "../engine/graphic/engine.hpp"

void particleEmitter::createParticleEmitter(ParticleEmitterType type) {
    emitter = GraphicEngine::Instance()->addParticleEmitter(type);
}

void particleEmitter::setPosition(float x, float y, float z) noexcept {
    emitter->setTranslation({x+offsetEmitter[0], y+offsetEmitter[1], z+offsetEmitter[2]});
    glm::vec3 a = emitter->getTranslation();
}

void particleEmitter::setOffset(float offsetX, float offsetY, float offsetZ) noexcept{
    offsetEmitter[0] = offsetX;
    offsetEmitter[1] = offsetY;
    offsetEmitter[2] = offsetZ;
}

void particleEmitter::setRotation(float x, float y, float z) noexcept {
    emitter->setRotation({x, y, z});
}

void particleEmitter::update(float dt) {
    auto position {emitter->getTranslation()};
    emitter->updateEmitter(dt, {position.x, position.y, position.z});
}

void particleEmitter::setActive(bool a) noexcept {
    emitter->setActive(a);
}

void particleEmitter::setActiveForTime(float time) noexcept {
    emitter->setActiveForTime(time);
}

void particleEmitter::setInterpolationValue(float pt) noexcept {
    emitter->setInterpolationValue(pt);
}

bool particleEmitter::getActive() const noexcept {
    return emitter->getActive();
}

entityPos particleEmitter::getPosition() const noexcept {
    auto pos {emitter->getTranslation()};
    return {pos.x, pos.y, pos.z};
}

GE::SceneParticleEmitter* particleEmitter::getEmitter() const noexcept {
    return emitter;
}

void particleEmitter::setPlayerReferencePosition(entityPos const& pos) noexcept {
    GE::SceneParticleEmitter::setPlayerReferencePosition(pos);
}