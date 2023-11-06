#pragma once

//#include <irrlicht/ICameraSceneNode.h>

struct GraphicEngine;
namespace GE{
    struct SceneCamera;
};


#define MAX_PITCH 1.55 // <PI/2
#define MIN_PITCH 4.75 // >3*PI/2
struct CameraNode {
    CameraNode();

    ~CameraNode();

    void createCamera(GraphicEngine*);
    void setOffsetY(float);
    void setMovementOffset(float);
    float getMovementOffset();
    void setPosition(float, float, float);
    void setRoll(float);
    void addYaw(float);
    void addPitch(float);
	void setAdditionalPitch(float);
	float getRoll();
    float getYaw();
    float getPitch(bool);
    void updatePositionAndRotation(float);
	void getCameraPosition(float*, float*, float*);
    float getCameraDistance();

    GE::SceneCamera* getCameraNode();
    
    private:
        //irr::scene::ICameraSceneNode* camera {nullptr};
        GE::SceneCamera* camera;
        float   roll{0.0},
                yaw{0.0},
                pitch{0.0};
		float	additionalPitch{};
        float   posX{},
                posY{},
                posZ{};
        float   offsetY{8},
                movementOffset{0},
                prevMovOffset{0};
        float   camDistance{-3.8};
};