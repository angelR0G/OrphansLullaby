#include "physicsEngine.hpp"
#include "../graphic/engine.hpp"
#include "collisionCallback.hpp"

#include <map>
// Collide with masks
const std::map<uint16_t, uint16_t> COLLIDE_WITH =
    {
        {SOLID_COLLISION,   PLAYER_COLLISION | ENEMY_COLLISION | ENEMIES_ATTACKS_COLLISION | DESTROYABLE_COLLISION | PROJECTILE_ROCKET_COLLISION},
        {TRIGGER_COLLISION, PLAYER_COLLISION | ENEMY_COLLISION},
        {ATTACK_COLLISION,  ENEMY_ATTACK_MASK | ENEMY_COLLISION},
        {PROJECTILE_SLOW_COLLISION, ENEMY_ATTACK_MASK | NO_TRASPASABLE_COLLISION},
        {PROJECTILE_SPIT_COLLISION, ENEMY_ATTACK_MASK | NO_TRASPASABLE_COLLISION},
        {NO_SIGHT_COLLISION, 0}
    };

PhysicsEngine::PhysicsEngine(bool debug) {
    phyWorld = phyCommon.createPhysicsWorld();
    
    if(debug) {
        using namespace reactphysics3d;
        phyWorld->setIsDebugRenderingEnabled(true);
        debuger = &(phyWorld->getDebugRenderer());
        debuger->setIsDebugItemDisplayed(DebugRenderer::DebugItem::COLLIDER_AABB            , false);
        debuger->setIsDebugItemDisplayed(DebugRenderer::DebugItem::COLLIDER_BROADPHASE_AABB , false);
        debuger->setIsDebugItemDisplayed(DebugRenderer::DebugItem::COLLISION_SHAPE          , true);
        debuger->setIsDebugItemDisplayed(DebugRenderer::DebugItem::CONTACT_NORMAL           , false);
        debuger->setIsDebugItemDisplayed(DebugRenderer::DebugItem::CONTACT_POINT            , false);

        // Get graphic engine
        device = GraphicEngine::Instance();
    }
}

PhysicsEngine::~PhysicsEngine(){
    startPointV.clear();
    finalPointV.clear();
    hitPointsV.clear();
};

// Create a new collision object to check collisions
void PhysicsEngine::addCollisionObject(CollisionComponent* cmp, Entity& entity, uint16_t mask) {
    reactphysics3d::Vector3     position(0.0, 0.0, 0.0); 
    reactphysics3d::Quaternion  orientation = reactphysics3d::Quaternion::identity(); 
    reactphysics3d::Transform   transform(position, orientation); 
    
    // Create a collision body in the world 
    reactphysics3d::CollisionBody* body; 
    body = phyWorld->createCollisionBody(transform);

    // Store the collision object in the component
    cmp->collision.setCollisionObject(body);

    // Adds extra information to the collision object
    cmp->collision.setCollisionData(&entity, mask);
    
    body->setUserData(cmp->collision.getUserData());
    
}

void PhysicsEngine::addCollisionObject(TriggerComponent* cmp, Entity& entity, uint16_t mask) {
    reactphysics3d::Vector3     position(0.0, 0.0, 0.0); 
    reactphysics3d::Quaternion  orientation = reactphysics3d::Quaternion::identity(); 
    reactphysics3d::Transform   transform(position, orientation); 
    
    // Create a collision body in the world 
    reactphysics3d::CollisionBody* body; 
    body = phyWorld->createCollisionBody(transform);

    // Store the collision object in the component
    cmp->collision.setCollisionObject(body);

    // Adds extra information to the collision object
    cmp->collision.setCollisionData(&entity, mask);
    
    body->setUserData(cmp->collision.getUserData());
    
}

// Creates a collision associated with no entity
CollisionObject PhysicsEngine::addCollisionObject(uint16_t mask, float x, float y, float z, float roll, float pitch) {
    reactphysics3d::Vector3     position(x, y, z); 
    reactphysics3d::Quaternion  orientation = reactphysics3d::Quaternion::fromEulerAngles({roll, pitch, 0.0}); 
    reactphysics3d::Transform   transform(position, orientation); 
    
    // Create a collision body in the world 
    reactphysics3d::CollisionBody* body; 
    body = phyWorld->createCollisionBody(transform);

    // Store the collision object in a new Collision object
    CollisionObject newCollision;
    newCollision.setCollisionObject(body);

    // Adds extra information to the collision object
    newCollision.setCollisionData(&newCollision, mask);
    
    body->setUserData(newCollision.getUserData());
    
    return newCollision;
}

// Delete an existing collision object
void PhysicsEngine::deleteCollisionObject(CollisionObject* c) {
    if(c->getCollisionObject() != nullptr)
        phyWorld->destroyCollisionBody(c->getCollisionObject());
    c->deleteCollisionData();
    c->setCollisionObject(nullptr);
}

// Adds a box collider to the given collision object
void PhysicsEngine::addBoxColliderToObject(CollisionObject* object, float x, float y, float z, float w, float h, float d, uint8_t flag) {
    using namespace reactphysics3d;

    // Creates the box shape
    BoxShape* boxShape = phyCommon.createBoxShape({w, h, d});

    addColliderToObject(object, boxShape, x, y, z, flag);
}

// Adds a capsule collider to the given collision object
void PhysicsEngine::addCapsuleColliderToObject(CollisionObject* object, float x, float y, float z, float radius, float height, uint8_t flag) {
    using namespace reactphysics3d;

    // Creates the capsule shape
    CapsuleShape* capsuleShape = phyCommon.createCapsuleShape(radius, height);

    addColliderToObject(object, capsuleShape, x, y, z, flag);
}

// Add a sphere collider to the given collision object
void PhysicsEngine::addSphereColliderToObject(CollisionObject* object, float x, float y, float z, float radius, uint8_t flag) {
    using namespace reactphysics3d;

    // Creates the sphere shape
    SphereShape* sphereShape = phyCommon.createSphereShape(radius);

    addColliderToObject(object, sphereShape, x, y, z, flag);
}

void PhysicsEngine::addColliderToObject(CollisionObject* object, reactphysics3d::CollisionShape* shape, float x, float y, float z, uint8_t flag) {
    using namespace reactphysics3d;

    // Set collider position relative to collision object position
    Transform transform = Transform::identity(); 
    transform.setPosition({x, y, z});
 
    // Create the collider and add it to the object 
    Collider* newCollider; 
    newCollider = object->getCollisionObject()->addCollider(shape, transform);

    // Set the collision category to the collider
    uint16_t collisionBits{object->getUserData()->flags};
    newCollider->setCollisionCategoryBits(collisionBits);

    // Set the collide with mask to the collider
    auto colWith = COLLIDE_WITH.find(collisionBits);
    if(colWith != COLLIDE_WITH.end()) {
        newCollider->setCollideWithMaskBits(colWith->second);
    }
    else {
        newCollider->setCollideWithMaskBits(ALL_COLLISION);
    }

    uint8_t* f =object->setFlags(flag);
    
    newCollider->setUserData(f);
}

//Check if two colliders overlap
bool PhysicsEngine::testOverlap(CollisionObject& c1, CollisionObject& c2) {
    return phyWorld->testOverlap(c1.getCollisionObject(), c2.getCollisionObject());
}

void PhysicsEngine::testCollision(CollisionObject& c1, CollisionObject& c2, CollisionDetect& callbackCollision){
    phyWorld->testCollision(c1.getCollisionObject(), c2.getCollisionObject(), callbackCollision);
}

void PhysicsEngine::testCollision(CollisionDetect& callback) {
    phyWorld->testCollision(callback);
}

// Render debug information about physic engine. PhysicsEngine must have been created with debug.
void PhysicsEngine::debugPhysics(const float& playerX, const float& playerY, const float& playerZ) {
    // Check if the debug is active
    if(debuger != nullptr && debugPhysicsEngine) {
        using namespace reactphysics3d;
        // Update physics world to update debug information
        phyWorld->update(1.0);

        // Render lines
        uint32_t iterations = debuger->getNbLines();
        if(iterations > 0) {
            const DebugRenderer::DebugLine* line = debuger->getLinesArray();

            while(iterations > 0) {
                if( closeToPlayer(playerX, playerY, playerZ, line->point1.x, line->point1.y, line->point1.z) || 
                    closeToPlayer(playerX, playerY, playerZ, line->point2.x, line->point2.y, line->point2.z)) {
                    device->draw3DLine(     line->point1.x, line->point1.y, line->point1.z, 
                                            line->point2.x, line->point2.y, line->point2.z);
                }

                iterations--;
                line++;
            }
        }
        
        // Render triangles
        iterations = debuger->getNbTriangles();
        if(iterations > 0) {
            const DebugRenderer::DebugTriangle* triangle = debuger->getTrianglesArray();

            while (iterations > 0) {
                if( closeToPlayer(playerX, playerY, playerZ, triangle->point1.x, triangle->point1.y, triangle->point1.z) || 
                    closeToPlayer(playerX, playerY, playerZ, triangle->point2.x, triangle->point2.y, triangle->point2.z) ||
                    closeToPlayer(playerX, playerY, playerZ, triangle->point3.x, triangle->point3.y, triangle->point3.z)) {
                    device->draw3DLine(  triangle->point1.x, triangle->point1.y, triangle->point1.z, 
                                        triangle->point2.x, triangle->point2.y, triangle->point2.z);
                    device->draw3DLine(  triangle->point3.x, triangle->point3.y, triangle->point3.z, 
                                        triangle->point2.x, triangle->point2.y, triangle->point2.z);
                    device->draw3DLine(  triangle->point1.x, triangle->point1.y, triangle->point1.z, 
                                        triangle->point3.x, triangle->point3.y, triangle->point3.z);
                }

                iterations--;
                triangle++;
            }
        }

        // Draw raycast info
        if(debugRaycast) {
            for(uint8_t i= 0; i<startPointV.size(); i++){
                if( closeToPlayer(playerX, playerY, playerZ, startPointV[i].x, startPointV[i].y, startPointV[i].z) ||
                    closeToPlayer(playerX, playerY, playerZ, finalPointV[i].x, finalPointV[i].y, finalPointV[i].z)) {
                    device->draw3DLine(  startPointV[i].x, startPointV[i].y, startPointV[i].z, 
                                        finalPointV[i].x, finalPointV[i].y, finalPointV[i].z);
                }
            }

            for(uint8_t i= 0; i<hitPointsV.size(); i++){
                if( closeToPlayer(playerX, playerY, playerZ, hitPointsV[i].x, hitPointsV[i].y, hitPointsV[i].z) ) {
                    device->draw3DLine(  hitPointsV[i].x, hitPointsV[i].y + 1, hitPointsV[i].z, 
                                        hitPointsV[i].x, hitPointsV[i].y - 1, hitPointsV[i].z);
                    device->draw3DLine(  hitPointsV[i].x + 1, hitPointsV[i].y, hitPointsV[i].z + 1, 
                                        hitPointsV[i].x - 1, hitPointsV[i].y, hitPointsV[i].z - 1);
                }
            }

        }
            
    }
}

bool PhysicsEngine::closeToPlayer(  const float& playerX, const float& playerY, const float& playerZ, 
                                    const float& x, const float& y, const float& z) {
    float distance {300.f};
    return std::abs(playerX - x) < distance && std::abs(playerY - y) < distance && std::abs(playerZ - z) < distance;
}

void PhysicsEngine::raycast(RayCast& callback, float x1, float y1, float z1, float x2, float y2, float z2) {
    // Create the raycast values
    reactphysics3d::Ray ray({x1, y1, z1}, {x2, y2, z2});

    // Calculate raycast
    phyWorld->raycast(ray, &callback);

    if(debuger != nullptr) {
        // If debug is active, save debug information
        debugRaycast        = true;
        collisionDetected   = callback.collision;
        hitPoints           = {callback.hitX, callback.hitY, callback.hitZ};
        startPoint          = {x1, y1, z1};
        finalPoint          = {x2, y2, z2};
        if(collisionDetected) hitPointsV.push_back(hitPoints);
        startPointV.push_back(startPoint);
        finalPointV.push_back(finalPoint);
        
        unsigned int maxRS = 2*3+1;
        // Clear raycast info
        if(startPointV.size() > maxRS || finalPointV.size() > maxRS || hitPointsV.size() > maxRS){
            startPointV.clear();
            finalPointV.clear();
            hitPointsV.clear();
        }
    }
}

void PhysicsEngine::freePhysicsData() {
    // Destroy physics world and create another new
    phyCommon.destroyPhysicsWorld(phyWorld);

    phyWorld = phyCommon.createPhysicsWorld();
    
    if(debuger != nullptr) {
        // Activate debug in the new physics world
        using namespace reactphysics3d;
        phyWorld->setIsDebugRenderingEnabled(true);
        debuger = &(phyWorld->getDebugRenderer());
        debuger->setIsDebugItemDisplayed(DebugRenderer::DebugItem::COLLIDER_AABB            , false);
        debuger->setIsDebugItemDisplayed(DebugRenderer::DebugItem::COLLIDER_BROADPHASE_AABB , false);
        debuger->setIsDebugItemDisplayed(DebugRenderer::DebugItem::COLLISION_SHAPE          , true);
        debuger->setIsDebugItemDisplayed(DebugRenderer::DebugItem::CONTACT_NORMAL           , false);
        debuger->setIsDebugItemDisplayed(DebugRenderer::DebugItem::CONTACT_POINT            , false);
    }
}

void PhysicsEngine::enableDebug(bool enable) noexcept {
    debugPhysicsEngine = enable;
}