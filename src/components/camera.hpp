#pragma once
#include "../engine/camera.hpp"

#define CAM_OFFSET_WALK     0.25
#define CAM_OFFSET_RUN      0.85

struct CameraComponent {
    CameraNode cameraNode{};
    bool offsetUp{true};
};