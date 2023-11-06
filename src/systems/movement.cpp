#include "movement.hpp"
#include "../components/movement.hpp"
#include "../utils/vectorMath.hpp"
#include "../engine/types.hpp"
#include <cmath>
#include "../Fmod/MasksPositions.hpp"

#define PI                  3.1415927
#define CAMERA_MOV_RUN      8

void MovementSystem::update(EntityMan& EM, const double dt, std::array<float, 3>* playerPos) {
    using CList = MetaP::Typelist<MovementComponent>;
    using TList = MetaP::Typelist<>;
    EM.foreach<CList, TList>([&](Entity& e){
        MovementComponent* movCmp   = &EM.getComponent<MovementComponent>(e);
        BasicComponent* basic       = &EM.getComponent<BasicComponent>(e);
        SoundComponent* sc {nullptr};

        if(e.hasComponent<SoundComponent>()){
            sc = &EM.getComponent<SoundComponent>(e);
        }

        if(e.template hasTag<PlayerTag>()) {
            // The entity is the player, save the position
            *playerPos = {basic->x, basic->y, basic->z};
        }

        // Check if entity is moving
        if (movCmp->moving) {
            if(e.template hasTag<EnemyNormalTag>() || e.template hasTag<EnemyExplosiveTag>() || e.template hasTag<EnemyRugbyTag>() || e.template hasTag<EnemyThrowerTag>()){
                if(sc != nullptr){
                    if((sc->maskSounds[M_ZOMBIE_STEPS] & SOUND_PLAYING) != SOUND_PLAYING){
                        sc->maskSounds[M_ZOMBIE_STEPS]          |=  SOUND_PLAY ;
                        sc->maskSounds[M_ZOMBIE_STEPS]          |=  SOUND_UPDATE_POSITION;
                    }
                    if(sc->paramValues.find("velocidad") != sc->paramValues.end()){
                        if(sc->paramValues.find("velocidad")->second !=1){
                            sc->paramValues.insert_or_assign("velocidad", 1);
                            sc->maskSounds[M_ZOMBIE_STEPS]          |=  SOUND_UPDATE_PARAM ;
                            sc->maskSounds[M_ZOMBIE_STEPS]          |=  SOUND_UPDATE_POSITION;
                        }else if(sc->paramValues.find("velocidad")->second ==1){
                            sc->maskSounds[M_ZOMBIE_STEPS]          |=  SOUND_UPDATE_POSITION;
                        }
                    }
                }
            }

            // Calculate direction of movement
            double movDirection = basic->orientation + (movCmp->movOrientation * 2*PI);
            if      (movDirection < 0)      movDirection += 2*PI;
            else if (movDirection > 2*PI)   movDirection -= 2*PI;

            // Player can run only moving forward
            if(movCmp->movOrientation >= -0.125 && movCmp->movOrientation <= 0.125 && movCmp->tryToRun)
                movCmp->run = true;
            else {
                movCmp->run         = false;
                movCmp->tryToRun    = false;
            }

            // Update linear velocity
            movCmp->vLinear += movCmp->accel*dt;

            if (movCmp->run && movCmp->vLinear > movCmp->maxRunVelocity)    movCmp->vLinear = movCmp->maxRunVelocity;
            else if (!movCmp->run && movCmp->vLinear > movCmp->maxVelocity) movCmp->vLinear = movCmp->maxVelocity;

            // Slow entity if has slow effect
            if(basic->slowEffect>0){
                movCmp->vLinear *= 0.8;
            }

            // Calculate velocity on each coordinate axis
            float newVX = movCmp->vLinear * sin(movDirection);
            float newVZ = movCmp->vLinear * cos(movDirection);
            movCmp->vX = movCmp->vX * movCmp->friction + (1-movCmp->friction) * newVX;
            movCmp->vZ = movCmp->vZ * movCmp->friction + (1-movCmp->friction) * newVZ;

            
            /*if the entity is the player first we make sure the steps arent playing already and then
            **we assign a speed to the steps event that is prepared to change the rythm based on the
            **actual player's speed**/
            if(e.template hasTag<PlayerTag>()){
                if(sc != nullptr){
                    if((sc->maskSounds[M_PLAYER_STEPS] & SOUND_PLAYING) != SOUND_PLAYING){
                        sc->maskSounds[M_PLAYER_STEPS]          |=  SOUND_PLAY ;
                    }
                    if(sc->paramValues.find("velocidad") != sc->paramValues.end()){
                        if(sc->paramValues.find("velocidad")->second != movCmp->vLinear){
                            sc->maskSounds[M_PLAYER_STEPS] |= SOUND_UPDATE_PARAM;
                            sc->paramValues.insert_or_assign("velocidad", movCmp->vLinear);
                        }
                    }
                }

                // Create camera movement
                CameraComponent* camCmp = &EM.getComponent<CameraComponent>(e);
                float currentOffset{camCmp->cameraNode.getMovementOffset()};

                // Check if is running
                if(movCmp->run) {
                    // Running camera tilts more and faster
                    if(camCmp->offsetUp) {
                        currentOffset += dt*CAMERA_MOV_RUN;

                        // Check if has to change direction
                        if(currentOffset >= CAM_OFFSET_RUN) {
                            currentOffset       = CAM_OFFSET_RUN;
                            camCmp->offsetUp    = false;
                        }
                    }
                    else {
                        currentOffset -= dt*CAMERA_MOV_RUN;

                        // Check if has to change direction
                        if(currentOffset <= -CAM_OFFSET_RUN) {
                            currentOffset       = -CAM_OFFSET_RUN;
                            camCmp->offsetUp    = true;
                        }
                    }
                }
                else {
                    // Walking camera tilts slowly
                    if(camCmp->offsetUp) {
                        currentOffset += dt;

                        // Check if has to change direction
                        if(currentOffset >= CAM_OFFSET_WALK) {
                            currentOffset       = CAM_OFFSET_WALK;
                            camCmp->offsetUp    = false;
                        }
                    }
                    else {
                        currentOffset -= dt;

                        // Check if has to change direction
                        if(currentOffset <= -CAM_OFFSET_WALK) {
                            currentOffset       = -CAM_OFFSET_WALK;
                            camCmp->offsetUp    = true;
                        }
                    }
                }

                // Update camera movement offset
                camCmp->cameraNode.setMovementOffset(currentOffset);
            }
        }
        else {
            // Reduces velocity
            double idt = std::min(1.0, 1-dt);
            movCmp->vLinear *= movCmp->friction * idt;
            movCmp->vX      *= movCmp->friction * idt;
            movCmp->vZ      *= movCmp->friction * idt;

            // Stop running
            movCmp->run         = false;
            movCmp->tryToRun    = false;
            if(e.template hasTag<PlayerTag>()){
                if(sc !=nullptr){
                    if(sc->paramValues.find("velocidad") != sc->paramValues.end()){
                        if(sc->paramValues.find("velocidad")->second != movCmp->vLinear && (sc->maskSounds[M_PLAYER_STEPS] & SOUND_PLAYING)){
                            sc->maskSounds[M_PLAYER_STEPS]      |=  SOUND_UPDATE_PARAM;
                            sc->maskSounds[M_PLAYER_STEPS]      |=  SOUND_STOP;
                            sc->paramValues.insert_or_assign("velocidad", movCmp->vLinear);
                        }
                    }
                }

                // Reduces camera movement to origin position
                CameraComponent* camCmp = &EM.getComponent<CameraComponent>(e);
                float currentOffset{camCmp->cameraNode.getMovementOffset()};

                if(currentOffset < 0.0) {
                    camCmp->offsetUp = true;
                    currentOffset   += dt;

                    if(currentOffset >= 0.0) {
                        currentOffset = 0.0;
                    }
                }
                else if(currentOffset > 0.0) {
                    camCmp->offsetUp = false;
                    currentOffset   -= dt;

                    if(currentOffset <= 0.0) {
                        currentOffset = 0.0;
                    }
                }

                // Update camera movement offset
                camCmp->cameraNode.setMovementOffset(currentOffset);
            }
            else if(e.template hasTag<EnemyNormalTag>() || e.template hasTag<EnemyExplosiveTag>() || e.template hasTag<EnemyRugbyTag>() || e.template hasTag<EnemyThrowerTag>()){
                if(sc != nullptr){
                    if(sc->paramValues.find("velocidad") != sc->paramValues.end()){
                        if(sc->paramValues.find("velocidad")->second != 0 && (sc->maskSounds[M_ZOMBIE_STEPS] & SOUND_PLAYING)){
                            sc->maskSounds[M_ZOMBIE_STEPS]      |=  SOUND_UPDATE_PARAM;
                            sc->maskSounds[M_ZOMBIE_STEPS]      |=  SOUND_STOP;
                            sc->paramValues.insert_or_assign("velocidad", 0);
                        }
                    }
                }
            }
        }

        // Entities that move vertically modify their Y velocity
        float newVY = 0;
        if(basic->verticalOrientation != 0) {
            newVY = movCmp->vLinear * sin(basic->verticalOrientation);

            // Recalculate each coordinate velocity to match linear velocity
            vectorMath::normalizeVector3D(&movCmp->vX, &newVY, &movCmp->vZ);

            movCmp->vX  *= movCmp->vLinear;
            movCmp->vZ  *= movCmp->vLinear;
            movCmp->vY  = newVY * movCmp->vLinear;
        }
        else {
            //Apply gravity
            if (movCmp->maxGravity > 0.1)
                movCmp->vY  = std::max(movCmp->vY - movCmp->gravity * dt, -movCmp->maxGravity);
        }

        basic->prevx = basic->x;
        basic->prevy = basic->y;
        basic->prevz = basic->z;

        basic->prevorientation = basic->orientation;

        // Update position
        basic->x    += movCmp->vX * dt;
        basic->y    += movCmp->vY * dt;
        basic->z    += movCmp->vZ * dt;

        if(basic->slowEffect>0){
            basic->slowEffect-=dt;
        }
    });
}