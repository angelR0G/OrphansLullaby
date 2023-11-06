#include "sceneCamera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "../engine.hpp"

GE::SceneCamera::SceneCamera(bool active) : SceneEntity(){
    if(active) activateCamera();
}

void GE::SceneCamera::draw(const glm::mat4& modelMatrix, ShaderResource* shader){}

void GE::SceneCamera::activateCamera() {
    GraphicEngine::Instance()->activeCamera = this;
}

glm::mat4 GE::SceneCamera::getViewMatrix() {
    return glm::inverse(getTransformMatrix());
}

glm::mat4 GE::SceneCamera::getProjectionMatrix() {
    // Check if the matrix must be recalculated
    if(updateProjection) {
        // Calculate projection matrix
        projectionMatrix = glm::perspective(glm::radians(camAngle), vpWidth/vpHeight, near, far*renderDistance);

        updateProjection = false;
    }

    return projectionMatrix;
}

void GE::SceneCamera::setProjectionNear(float n){
    near                = n;
    updateProjection    = true;
}

void GE::SceneCamera::setProjectionFar(float f){
    far                 = f;
    updateProjection    = true;
}

void GE::SceneCamera::setProjectionViewportSize(float w, float h){
    vpWidth             = w;
    vpHeight            = h;
    updateProjection    = true;
}

void GE::SceneCamera::setProjectionCameraAngle(float angle){
    camAngle            = angle;
    updateProjection    = true;
}

void GE::SceneCamera::setUpdateProjection() noexcept {
    updateProjection    = true;
}

void GE::SceneCamera::setRenderDistance(float distance) noexcept {
    renderDistance      = distance;
}

std::pair<float, float> GE::SceneCamera::getNearFarPlanes() const noexcept{
    return std::pair<float, float>{near, far};
}

float GE::SceneCamera::getCamAngle() const noexcept{
    return camAngle;
}

float GE::SceneCamera::getWindowAspect() const noexcept{
    return (vpWidth/vpHeight);
}

glm::vec3 GE::SceneCamera::getUpVector() const noexcept{
    return upVector;
}