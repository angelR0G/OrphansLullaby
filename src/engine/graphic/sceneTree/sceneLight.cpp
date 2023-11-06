#include "sceneLight.hpp"
#include "../engine.hpp"

GE::SceneLight::SceneLight() : SceneEntity(){

}

void GE::SceneLight::draw(const glm::mat4& modelMatrix, ShaderResource* shader){}

void GE::SceneLight::setActive(bool a) {
    active = a;
}

void GE::SceneLight::setType(uint8_t t) {
    // Type must be a value between 1 and 3
    type = min(max(t, (uint8_t)1), (uint8_t)3);
}

void GE::SceneLight::setIntensity(float i) {
    // Intensity must be a value between 0 and 1
    intensity = min(max(i, 0.0f), 1.0f);
}

void GE::SceneLight::setRadius(float r) {
    // Radius must be greater than 0
    radius = max(r, 0.01f);
}

void GE::SceneLight::setColor(glm::vec3 c) {
    color = c;
}

void GE::SceneLight::setDirection(glm::vec3 d) {
    direction = normalize(d);
}

void GE::SceneLight::setInnerAngle(float iAngle) {
    // Inner angle must be grater than 0
    innerAngle = std::max(iAngle, 0.01f);
}

void GE::SceneLight::setOuterAngle(float oAngle) {
    // Outer angle must be grater than 0
    outerAngle = std::max(oAngle, 0.01f);
}

void GE::SceneLight::setCastShadows(bool cast) {
    castShadows = cast;
}

bool GE::SceneLight::getActive() const {
    return active;
}

uint8_t GE::SceneLight::getType() const {
    return type;
}

float GE::SceneLight::getIntensity() const {
    return intensity;
}

float GE::SceneLight::getRadius() const {
    return radius;
}

glm::vec3 GE::SceneLight::getColor() const {
    return color;
}

glm::vec3 GE::SceneLight::getDirection() const {
    return direction;
}

float GE::SceneLight::getInnerAngle() const {
    return innerAngle;
}

float GE::SceneLight::getOuterAngle() const {
    return outerAngle;
}

bool GE::SceneLight::getCastShadows() const {
    return castShadows;
}