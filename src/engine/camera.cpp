#include "camera.hpp"
//#include <irrlicht/ISceneManager.h>
#include <irrlicht/matrix4.h>
#include "../engine/graphic/sceneTree/sceneCamera.hpp"
#include "../engine/graphic/engine.hpp"

#define PI              3.1415926
#define YAW_CORRECTION  PI

CameraNode::CameraNode() {}

CameraNode::~CameraNode() {}

void CameraNode::createCamera(GraphicEngine* gE) {
    if (camera == nullptr)  {
        camera = gE->addSceneCamera();
    }
}

void CameraNode::setOffsetY(float y) {
    offsetY = y;

    return;
}

void CameraNode::setMovementOffset(float y) {
    prevMovOffset   = movementOffset;
    movementOffset  = y;

    return;
}

float CameraNode::getMovementOffset() {
    return movementOffset;
}

void CameraNode::setPosition(float x, float y, float z) {
    posX = x;
    posY = y;
    posZ = z;

    return;
}

void CameraNode::setRoll(float r) {
    roll = r;
    if (roll > 2*PI)    roll -= 2*PI;
    else if (roll < 0)  roll += 2*PI;

    return;
}

void CameraNode::addYaw(float y) {
    yaw += y;
    if (yaw > 2*PI)     yaw -= 2*PI;
    else if (yaw < 0)   yaw += 2*PI;

    return;
}

void CameraNode::addPitch(float p) {
    pitch -= p;
    if (pitch > 2*PI)   pitch -= 2*PI;
    else if (pitch < 0) pitch += 2*PI;

    // Check camera movement vertical limitation
    if (pitch > MAX_PITCH && pitch < MIN_PITCH) {
        if (p >= 0.0)   pitch = MIN_PITCH;
        else            pitch = MAX_PITCH;
    }

    return;
}

void CameraNode::setAdditionalPitch(float p) {
    additionalPitch = p;
}

void CameraNode::updatePositionAndRotation(float percentTick) {
    if(camera == nullptr) return;

    // Calculate position
    float   currentX    = posX,
            currentY    = posY + offsetY + prevMovOffset + (movementOffset-prevMovOffset)*percentTick,
            currentZ    = posZ;
    float   finalPitch  = getPitch(true);

    // Add camera horizontal offset
    currentX += camDistance * sin(yaw) * cos(finalPitch);
    currentZ += camDistance * cos(yaw) * cos(finalPitch);

    // Update camera position and target position
    camera->setTranslation(glm::vec3(currentX, currentY, currentZ));
    camera->setRotation(glm::vec3(finalPitch, yaw + YAW_CORRECTION, roll));

    return;
}

void CameraNode::getCameraPosition(float* x, float* y, float* z) {
    //irr::core::vector3df pos = camera->getAbsolutePosition();
    glm::vec3 pos = camera->getTranslation();
    *x = pos.x;
    *y = pos.y;
    *z = pos.z;
}

float CameraNode::getRoll() {
    return roll;
}

// Get yaw rotation (horizontal)
float CameraNode::getYaw() {
    return yaw;
}

// Get pitch rotation (vertical). Send true to take into account additional pitch.
float CameraNode::getPitch(bool additional) {
    if(additional) {
        float   finalPitch  = pitch + additionalPitch;
        if (finalPitch > MAX_PITCH && finalPitch < MIN_PITCH) finalPitch = MAX_PITCH;
        return finalPitch;
    }
    else
        return pitch;
}

GE::SceneCamera* CameraNode::getCameraNode() {
    return camera;
}

float CameraNode::getCameraDistance() {
    return camDistance;
}