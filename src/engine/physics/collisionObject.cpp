#include "collisionObject.hpp"
#include "utils/collisionTypes.hpp"
#include <cassert>

std::array<CollisionObjectData, MAX_COLLISIONS> CollisionObject::data {};
std::array<uint8_t, 5>                          CollisionObject::flags{COLLIDER_NORMAL, COLLIDER_HEAD, COLLIDER_Trigger};
size_t                                          CollisionObject::freespace{0};

CollisionObject::CollisionObject() {}

CollisionObject::~CollisionObject() {}

void CollisionObject::setCollisionObject(reactphysics3d::CollisionBody* c) {
    collisionObject = c;
}

reactphysics3d::CollisionBody* CollisionObject::getCollisionObject() {
    return collisionObject;
}

void CollisionObject::setTransform(float x, float y, float z, float yaw, float pitch) {
    using namespace reactphysics3d;

    if(collisionObject == nullptr) return;
    
    // Especify collision transform
    Vector3 position(x, y, z); 
    Quaternion orientation = Quaternion::fromEulerAngles({pitch, yaw, 0});
    Transform newTransform(position, orientation); 
    
    // Update collision position and orientation
    collisionObject->setTransform(newTransform);
}

void CollisionObject::setRotation(float r, float y, float p) {
    using namespace reactphysics3d;
    // Get current position
    Vector3 pos = collisionObject->getWorldPoint({0, 0, 0});
    
    // Update rotation
    Quaternion orientation = Quaternion::fromEulerAngles({r, y, p});
    Transform t (pos, orientation);

    // Update collision transform
    collisionObject->setTransform(t);
}

void CollisionObject::deleteCollisionData() {
    if(coll_data != nullptr) {
        coll_data->entity = nullptr;
    }
    coll_data = nullptr;
}

CollisionObjectData* CollisionObject::getUserData() {
    return coll_data;
}

//Store the entity pointer and flags for collisions in the collisiondata array
void CollisionObject::setCollisionData(void* pointer, uint16_t f) {
    bool noSpace{false};
    //Check if the freespace is empty
    while(data[freespace].entity != nullptr){
        ++freespace;
        if(freespace == data.size()) {
            freespace = 0;
            if(!noSpace)    noSpace = true;
            else            assert("Collision data array has not enough space");
        }
        
    }
    data[freespace].entity = pointer;
    data[freespace].flags  = f;
    coll_data = &data[freespace];
}

void CollisionObject::updateEntity(void* pointer) {
    coll_data->entity = pointer;
}

uint8_t* CollisionObject::setFlags(uint8_t flag){
    return &flags[flag];
}
