#include "camera.hpp"
#include "../components/camera.hpp"

void CameraSystem::update(EntityMan& EM, float percentTick) {
    using CList = MetaP::Typelist<CameraComponent>;
    using TList = MetaP::Typelist<>;
    EM.foreach<CList, TList>([&](Entity& e){
        CameraComponent* camCmp     = &EM.getComponent<CameraComponent>(e);
        BasicComponent* basicCmp    = &EM.getComponent<BasicComponent>(e);

        float ix = basicCmp->prevx * (1-percentTick) + basicCmp->x * percentTick;
        float iy = basicCmp->prevy * (1-percentTick) + basicCmp->y * percentTick;
        float iz = basicCmp->prevz * (1-percentTick) + basicCmp->z * percentTick;
        camCmp->cameraNode.setPosition(ix, iy, iz);
        camCmp->cameraNode.updatePositionAndRotation(percentTick);
    });
}