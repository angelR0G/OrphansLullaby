#include "weapon.hpp"
#include "../manager/weaponStats.hpp"
#include "../engine/entityFactory.hpp"
#include "../engine/physics/physicsEngine.hpp"
#include "../Fmod/MasksPositions.hpp"
#include "../utils/stressLevel.hpp"
#include "../utils/WeaponTemplates.hpp"

#define MAX_IDLE_OFFSET      0.1
#define IDLE_OFFSET_Y_INC    0.03
#define IDLE_OFFSET_X_INC    0.01
#define IDLE_PAUSE_TIME      1.2
#define RAD_TO_DEG           (float)(180.0/3.1415927)
#define RECOIL_ADD           1.4
#define RECOIL_SUB           2.2
#define SHOT_DISTANCE        600
#define ROCKET_INITIAL_DIST  5
#define SHOTGUN_DISTANCE     180.f
#define HEADSHOOT_MULTIPLIER 2

#define WEAPON_DOWN         8    
#define HITMARK_TIME        0.25
#define CHANGE_WEAPON_TIME  0.5

// Shotgun dispersion
#define SG_X_MIDDLE         0.0523599
#define SG_Y_MIDDLE         0.0698132
#define SG_X_LATERAL        0.139626

const std::vector<uint16_t> shootSounds = {M_PISTOL_SHOOT, M_REVOLVER_SHOOT, M_APISTOL_SHOOT, M_LMACHINEGUN_SHOOT, M_M16_SHOOT,
M_M1_SHOOT, M_LASER_SHOOT, M_CSHOTGUN_SHOOT, M_SHOTGUN_SHOOT, M_HMACHINEGUN_SHOOT, M_BAZOOKA_SHOOT};
const std::vector<uint16_t> reloadSounds = {M_PISTOL_RELOAD, M_REVOLVER_RELOAD, M_PISTOL_RELOAD, M_LMACHINEGUN_RELOAD, M_M16_RELOAD,
M_M1_RELOAD, M_LASER_RELOAD, M_CSHOTGUN_RELOAD, M_SHOTGUN_RELOAD, M_HMACHINEGUN_RELOAD, M_BAZOOKA_RELOAD};
    
void WeaponSystem::update(EntityMan& EM, PhysicsEngine& physics, const double dt, weaponStats& wStats, StressLevel& stress) {
    using CList = MetaP::Typelist<WeaponComponent,CameraComponent,SoundComponent,MovementComponent>;
    using TList = MetaP::Typelist<>;
    EM.foreach<CList, TList>([&](Entity& e){
        WeaponComponent* wCmp       = &EM.getComponent<WeaponComponent>(e);
        MovementComponent* movCmp   = &EM.getComponent<MovementComponent>(e);
        CameraComponent* camCmp     = &EM.getComponent<CameraComponent>(e);
        SoundComponent* sound       = &EM.getComponent<SoundComponent>(e);

        WeaponData* weapon = wCmp->activeWeapon;

        if(wCmp->hitEnemy > 0) wCmp->hitEnemy -= dt;

        // Get camera position and orientation for later use
        CameraNode* camera = &camCmp->cameraNode;
        float camX, camY, camZ;
        camera->getCameraPosition(&camX, &camY, &camZ);
        float camYaw    = camera->getYaw();
        float camPitch  = camera->getPitch(true);

        uint8_t* weaponState = &weapon->weaponState;

        int collisionType = 0;
        auto playerChangeWeapon = [](const WeaponComponent* wCmp)   {return wCmp->changeWeapon | wCmp->getFirstWeapon | wCmp->getSecondWeapon;};
        auto shotCooldown       = [](const WeaponData* weapon)      {return weapon->lastShot < weapon->fireRate;};
        
        // Check if is changing the weapon
        if(wCmp->changeWeaponTime > 0) {
            wCmp->changeWeaponTime -= dt;
        }
        else if(playerChangeWeapon(wCmp) && !shotCooldown(weapon)) {
            // Try to change weapon
            if(wCmp->changeWeapon)          changeWeapon(e, wCmp, sound);
            else if(wCmp->getFirstWeapon)   changeWeapon(e, wCmp, false, sound);
            else if(wCmp->getSecondWeapon)  changeWeapon(e, wCmp, true, sound);
            weapon = wCmp->activeWeapon;
        }
        else {
            // Check if is trying to reload the weapon
            if (*weaponState & WEAPON_RELOAD && !shotCooldown(weapon)) {
                reloadAmmo(EM, e);
            }

            // Check if is reloading weapon
            if (weapon->reloadElapsed < weapon->reloadTime) {
                weapon->reloadElapsed += dt;

                // If reload has finished, remove reload state and fill ammo
                if(weapon->reloadElapsed >= weapon->reloadTime) {
                    // Remove reloading state
                    *weaponState &= ~WEAPON_RELOAD;

                    // Reload ammo
                    int reloadAmmo = std::min(weapon->magSize - weapon->ammo, (int)weapon->totalAmmo);

                    weapon->ammo        += reloadAmmo;
                    weapon->totalAmmo   -= reloadAmmo;
                }
            }
            // Check fire rate and last shot time
            else if (shotCooldown(weapon)) {
                weapon->lastShot += dt;

                // If releases trigger, change shot state
                if (*weaponState & WEAPON_FIRING)   *weaponState |= WEAPON_SHOT;
                else                                *weaponState &= ~WEAPON_SHOT;
            }
            else {
                // Check if entity is trying to shot
                // Cannot shot while running or changing weapon
                if ((*weaponState & WEAPON_FIRING) && !movCmp->run && wCmp->changeWeaponTime < 0.0001) {
                    // Check if there is ammo left
                    if (weapon->ammo > 0) {
                        // Check shot type
                        if (!(*weaponState & WEAPON_SINGLE_SHOT) || !(*weaponState & WEAPON_SHOT)) {
                            /*********************
                            ******* Shoot ********
                            *********************/
                            // Add recoil
                            *weaponState |= WEAPON_SHOT | WEAPON_ADD_RECOIL;
                            wCmp->maxRecoilAdded += weapon->recoilForce;
                            if (wCmp->maxRecoilAdded > weapon->maxRecoilForce) wCmp->maxRecoilAdded = weapon->maxRecoilForce;

                            //Shot animation
                            if(e.hasComponent<AnimationComponent>()){
                                //Activate shot animation
                                AnimationComponent* animC = &EM.getComponent<AnimationComponent>(e);
                                if(animC != nullptr){
                                    animC->rAnimation = static_cast<int>(RunningAnimationWeapon::SHOOT_ANIMATION);
                                }
                            }

                            // Play shot sound                            
                            sound->maskSounds[shootSounds[weapon->weaponID-1]] |= SOUND_PLAY;
                            if(weapon->weaponID == 6 && weapon->ammo == 1){
                                sound->maskSounds[shootSounds[weapon->weaponID-1]] |= SOUND_UPDATE_PARAM;
                                sound->paramValues.insert_or_assign("eyeccion", 1);

                                if(e.hasComponent<AnimationComponent>()){
                                    //Activate shot animation for rifle last shot
                                    AnimationComponent* animC = &EM.getComponent<AnimationComponent>(e);
                                    if(animC != nullptr){
                                        animC->rAnimation = static_cast<int>(RunningAnimationRifle::LAST_SHOOT_ANIMATION);
                                    }
                                }
                            }

                            
                            // Check weapon type to cast rays
                            if(*weaponState & WEAPON_SHOTGUN) {
                                if(e.hasComponent<AnimationComponent>()){
                                    //Activate attack animation
                                    AnimationComponent* animC = &EM.getComponent<AnimationComponent>(e);
                                    if(animC != nullptr){
                                        animC->rAnimation = static_cast<int>(RunningAnimationWeapon::SHOOT_ANIMATION);
                                    }
                                }
                                int hits{0};
                                RayCast ray;  
                                ray.filterMask = SHOT_COLLISION_MASK;
                                // Shotgun, creates 7 raycasts for every shot
                                float   targetX     { camX + SHOTGUN_DISTANCE * sinf(camYaw) * cosf(camPitch)},
                                        targetZ     { camZ + SHOTGUN_DISTANCE * cosf(camYaw) * cosf(camPitch)},
                                        targetY     { camY + SHOTGUN_DISTANCE * sinf(camPitch)};

                                // Cast ray to check shot collision
                                physics.raycast(ray, camX, camY, camZ, targetX, targetY, targetZ);
                                hits = std::max(checkShotGunCollision(&ray, e, EM, weapon->damage * 2, true), hits);
                                ray.reset();

                                // Second
                                targetX = camX + SHOTGUN_DISTANCE * sinf(camYaw + SG_X_LATERAL) * cosf(camPitch);
                                targetZ = camZ + SHOTGUN_DISTANCE * cosf(camYaw + SG_X_LATERAL) * cosf(camPitch);

                                physics.raycast(ray, camX, camY, camZ, targetX, targetY, targetZ);
                                hits = std::max(checkShotGunCollision(&ray, e, EM, weapon->damage), hits);
                                ray.reset();

                                // Third
                                targetX = camX + SHOTGUN_DISTANCE * sinf(camYaw - SG_X_LATERAL) * cosf(camPitch);
                                targetZ = camZ + SHOTGUN_DISTANCE * cosf(camYaw - SG_X_LATERAL) * cosf(camPitch);

                                physics.raycast(ray, camX, camY, camZ, targetX, targetY, targetZ);
                                hits = std::max(checkShotGunCollision(&ray, e, EM, weapon->damage), hits);
                                ray.reset();

                                // Fourth
                                targetX = camX + SHOTGUN_DISTANCE * sinf(camYaw + SG_X_MIDDLE) * cosf(camPitch + SG_Y_MIDDLE);
                                targetZ = camZ + SHOTGUN_DISTANCE * cosf(camYaw + SG_X_MIDDLE) * cosf(camPitch + SG_Y_MIDDLE);
                                targetY = camY + SHOTGUN_DISTANCE * sinf(camPitch + SG_Y_MIDDLE);

                                physics.raycast(ray, camX, camY, camZ, targetX, targetY, targetZ);
                                hits = std::max(checkShotGunCollision(&ray, e, EM, weapon->damage + weapon->damage/2), hits);
                                ray.reset();

                                // Fifth
                                targetX = camX + SHOTGUN_DISTANCE * sinf(camYaw - SG_X_MIDDLE) * cosf(camPitch + SG_Y_MIDDLE);
                                targetZ = camZ + SHOTGUN_DISTANCE * cosf(camYaw - SG_X_MIDDLE) * cosf(camPitch + SG_Y_MIDDLE);

                                physics.raycast(ray, camX, camY, camZ, targetX, targetY, targetZ);
                                hits = std::max(checkShotGunCollision(&ray, e, EM, weapon->damage + weapon->damage/2), hits);
                                ray.reset();

                                // Sixth
                                targetX = camX + SHOTGUN_DISTANCE * sinf(camYaw + SG_X_MIDDLE) * cosf(camPitch - SG_Y_MIDDLE);
                                targetZ = camZ + SHOTGUN_DISTANCE * cosf(camYaw + SG_X_MIDDLE) * cosf(camPitch - SG_Y_MIDDLE);
                                targetY = camY + SHOTGUN_DISTANCE * sinf(camPitch - SG_Y_MIDDLE);

                                physics.raycast(ray, camX, camY, camZ, targetX, targetY, targetZ);
                                hits = std::max(checkShotGunCollision(&ray, e, EM, weapon->damage + weapon->damage/2), hits);
                                ray.reset();

                                // Seventh
                                targetX = camX + SHOTGUN_DISTANCE * sinf(camYaw - SG_X_MIDDLE) * cosf(camPitch - SG_Y_MIDDLE);
                                targetZ = camZ + SHOTGUN_DISTANCE * cosf(camYaw - SG_X_MIDDLE) * cosf(camPitch - SG_Y_MIDDLE);

                                physics.raycast(ray, camX, camY, camZ, targetX, targetY, targetZ);
                                hits = std::max(checkShotGunCollision(&ray, e, EM, weapon->damage + weapon->damage/2), hits);

                                if(hits > 0) {
                                    sound->maskSounds[M_HITMARKER] |= SOUND_PLAY;
                                    collisionType = hits;
                                }
                            }
                            else if(*weaponState & WEAPON_ROCKET) {
                                BasicComponent* basicCmp = &EM.getComponent<BasicComponent>(e);
                                std::vector<float> atparams { 0, 0, 0, 2.0f, 2.0f, 2.0f, weapon->damage, 0, 10};

                                // Create the rocket
                                Entity* rocket = EntityFactory::Instance()->createProjectileAttack(basicCmp, atparams, ENEMY_COLLISION | PLAYER_COLLISION | DESTROYABLE_COLLISION, PROJECTILE_ROCKET_COLLISION);
                            
                                // Set its position and orientation
                                BasicComponent* rocketMov = &EM.getComponent<BasicComponent>(*rocket);

                                rocketMov->x = camX + ROCKET_INITIAL_DIST * sinf(camYaw) * cosf(camPitch);
                                rocketMov->z = camZ + ROCKET_INITIAL_DIST * cosf(camYaw) * cosf(camPitch);
                                rocketMov->y = camY + ROCKET_INITIAL_DIST * sinf(camPitch);

                                rocketMov->prevx = rocketMov->x;
                                rocketMov->prevy = rocketMov->y;
                                rocketMov->prevz = rocketMov->z;

                                rocketMov->verticalOrientation = camPitch;
                            }
                            else {
                                // Normal shot, calculate shot direction and make a raycast
                                float   targetX     { camX + SHOT_DISTANCE * sinf(camYaw) * cosf(camPitch)},
                                        targetZ     { camZ + SHOT_DISTANCE * cosf(camYaw) * cosf(camPitch)},
                                        targetY     { camY + SHOT_DISTANCE * sinf(camPitch)};

                                // Cast ray to check shot collision
                                RayCast ray;  
                                ray.filterMask = SHOT_COLLISION_MASK;
                                physics.raycast(ray, camX, camY, camZ, targetX, targetY, targetZ);
                                collisionType = checkShotCollision(&ray, e, EM);
                            }

                            weapon->ammo        -= 1;
                            weapon->lastShot    = 0;
                        }
                    }
                    else {
                        // No ammo left, reload automatically
                        *weaponState |= WEAPON_RELOAD;
                        sound->maskSounds[M_WEAPON_NOAMMO] |= SOUND_PLAY;
                    }
                }
                else
                    *weaponState &= ~WEAPON_SHOT;
            }
        }

        // Update weapon model position
        // Calculate idle offset
        if (wCmp->weaponIdlePause <= 0.0) {
            if (wCmp->weaponIdleIncrease) {
                if(wCmp->weaponIdleY < -MAX_IDLE_OFFSET)
                    wCmp->weaponIdleY = -MAX_IDLE_OFFSET;

                wCmp->weaponIdleY += IDLE_OFFSET_Y_INC * dt;
                wCmp->weaponIdleX += IDLE_OFFSET_X_INC * dt;

                if (wCmp->weaponIdleY > MAX_IDLE_OFFSET) {
                    wCmp->weaponIdleIncrease = false;
                    wCmp->weaponIdlePause = IDLE_PAUSE_TIME;
                }
            }
            else {
                if(wCmp->weaponIdleY > MAX_IDLE_OFFSET)
                    wCmp->weaponIdleY = MAX_IDLE_OFFSET;

                wCmp->weaponIdleY -= IDLE_OFFSET_Y_INC * dt;
                wCmp->weaponIdleX -= IDLE_OFFSET_X_INC * dt;

                if (wCmp->weaponIdleY < -MAX_IDLE_OFFSET) {
                    wCmp->weaponIdleIncrease = true;
                    wCmp->weaponIdlePause = IDLE_PAUSE_TIME;
                }
            }
        }
        else 
            wCmp->weaponIdlePause -= dt;

        // Adds recoil if has shot
        float*  currentRecoil   = &wCmp->currentRecoil;
        float*  maxRecoil       = &wCmp->maxRecoilAdded;
        float*  recoilReduction = &wCmp->recoilReduction;
        if (*maxRecoil > 0.0) {
            if(*weaponState & WEAPON_ADD_RECOIL) {
                *recoilReduction = 0.0;
                *currentRecoil += RECOIL_ADD * dt;
                if(*currentRecoil >= *maxRecoil) {
                    *currentRecoil = *maxRecoil;
                    *weaponState &= ~WEAPON_ADD_RECOIL;
                }
            }
            else {
                *recoilReduction += RECOIL_SUB * dt;
                *maxRecoil -= *recoilReduction * dt;
                *currentRecoil = *maxRecoil;
                if(*currentRecoil <= 0.0) {
                    *currentRecoil = 0.0;
                }
            }
            camCmp->cameraNode.setAdditionalPitch(*currentRecoil);
        }

        // Use camera rotation and position to update weapon position
        // Calculate position to move the weapon to the screen center
        camX += weapon->weaponDistance * sin(camYaw) * cos(camPitch);
        camZ += weapon->weaponDistance * cos(camYaw) * cos(camPitch);
        camY += weapon->weaponDistance * sin(camPitch);

        // Calculate offset to move the weapon to its position
        float offsetX, offsetY;
        if (wCmp->changeWeaponTime > 0) {
            double factor {wCmp->changeWeaponTime/CHANGE_WEAPON_TIME};
            wCmp->weaponYOffset = factor * -8 + weapon->weaponDownOffsetY;
        }
        else if (movCmp->run && weapon->reloadElapsed >= weapon->reloadTime) {
            wCmp->weaponYOffset -= dt * WEAPON_DOWN;
            if(wCmp->weaponYOffset < weapon->weaponDownOffsetY) wCmp->weaponYOffset = weapon->weaponDownOffsetY;
        }
        else {
            wCmp->weaponYOffset += dt * WEAPON_DOWN;
            if(wCmp->weaponYOffset > 0) wCmp->weaponYOffset = 0;
        }
        offsetX = weapon->weaponOffsetX + wCmp->weaponIdleX;
        offsetY = weapon->weaponOffsetY + wCmp->weaponIdleY + wCmp->weaponYOffset;

        camX += offsetX * cos(-camYaw) + offsetY * sin(-camPitch) * sin(camYaw);
        camZ += offsetX * sin(-camYaw) + offsetY * sin(-camPitch) * cos(-camYaw);
        camY += offsetY * cos(-camPitch);

        // Updates position and rotation
        weapon->weaponNode->setPosition(camX, camY, camZ);
        weapon->weaponNode->setRotation(-camPitch, camYaw, 0);

        // Save information for HUD display
        wStats.hitEnemy         = wCmp->hitEnemy;
        wStats.reloadTime       = weapon->reloadTime;
        wStats.reloadElapsed    = weapon->reloadElapsed;
        wStats.totalAmmo        = weapon->totalAmmo;
        wStats.ammo             = weapon->ammo;
        wStats.magSize          = weapon->magSize;
        wStats.infiniteAmmo     = weapon->weaponState & INFINITE_WEAPON;
        
        // Check if shot impacted on enemy
        if(collisionType != 0) {
            // Save collision type (normal/headshot)
            wStats.collisionType = collisionType;

            // Add stress
            stress.addStress(weapon->fireRate);
        }
    });
}

void WeaponSystem::createParticlesOnHit(Entity& e, RayCast* ray, EntityMan& EM){
    if(e.hasComponent<RenderComponent>()){
        RenderComponent* rComp      = &EM.getComponent<RenderComponent>(e);
        rComp->emitter.setOffset(ray->hitX, ray->hitY, ray->hitZ);
        rComp->emitter.setActiveForTime(0.2);
    }
}

//Returns 0 on no collision, 1 on normal collision, 2 on vulnerable collision
int WeaponSystem::checkShotCollision(RayCast* ray, Entity& e, EntityMan& EM) {
    int collide = 0;
    // Create a ray that only can collide with enemies and walls
    if(ray->collision){
        // Check if collided entity is an enemy
        Entity* hitEntity = static_cast<Entity*>(ray->entity);
        if(ray->bodyflags & ENEMY_COLLISION){
            // Shows hit marker
            WeaponComponent* wCmp = &EM.getComponent<WeaponComponent>(e);
            wCmp->hitEnemy = HITMARK_TIME;

            //Create particles on hit position
            createParticlesOnHit(e, ray, EM);

            // Reduce their health
            if(hitEntity->hasComponent<HealthComponent>()){
                HealthComponent* healthCmp      = &EM.getComponent<HealthComponent>(*hitEntity);
                SoundComponent* enemSoundCmp    = &EM.getComponent<SoundComponent>(*hitEntity);
                BasicComponent* enemBasicCmp    = &EM.getComponent<BasicComponent>(*hitEntity);
                SoundComponent* soundCmp        = &EM.getComponent<SoundComponent>(e);

                // Play sound
                enemSoundCmp->maskSounds[M_ZOMBIE_PAIN] |= SOUND_PLAY;
                soundCmp->maskSounds[M_HITMARKER] |= SOUND_PLAY;
                soundCmp->maskSounds[M_BULLET_MEATIMPACT] |= SOUND_PLAY;

                float damage {wCmp->activeWeapon->damage};
                // Check if has collided in vulnerable collider
                if(ray->colliderflags & COLLIDER_HEAD){
                    // Vulnerable collider, deals more damage
                    damage *= HEADSHOOT_MULTIPLIER;
                    collide = 2;
                }else {
                    // Normal collider, deals normal damage
                    collide = 1;
                }

                // Check if has damage resistance
                if(hasSpecialEffect(enemBasicCmp->spEffects, SpecialEffect::Tank)) 
                    healthCmp->health -= damage * SP_EFFECT_DAMAGE_REDUCTION_VALUE;
                else
                    healthCmp->health -= damage;
            }

            // Mark head shot
            if(collide == 2)    hitEntity->addTag<HeadShotMarkTag>();
            else                hitEntity->removeTag<HeadShotMarkTag>();
        }
        else if(ray->bodyflags & (BARREL_COLLISION | DESTROYABLE_COLLISION)){
            if(hitEntity->hasComponent<HealthComponent>()){
                HealthComponent* healthCmp  = &EM.getComponent<HealthComponent>(*hitEntity);
                WeaponComponent* wCmp       = &EM.getComponent<WeaponComponent>(e);
                // Normal collider, deals normal damage
                healthCmp->health -= wCmp->activeWeapon->damage;

                // Shows hit marker
                wCmp->hitEnemy = HITMARK_TIME;

                collide = 1;
            }
        }
    }

    return collide;
}

int WeaponSystem::checkShotGunCollision(RayCast* ray, Entity& e, EntityMan& EM, float damage, bool moreKillPoints) {
    int enemyHit{0};

    // Create a ray that only can collide with enemies and walls
    if(ray->collision){
        // Check if collided entity is an enemy
        Entity* hitEntity = static_cast<Entity*>(ray->entity);
        if(ray->bodyflags & ENEMY_COLLISION){
            // Shows hit marker
            WeaponComponent* wCmp = &EM.getComponent<WeaponComponent>(e);
            wCmp->hitEnemy = HITMARK_TIME;

            // Reduce their health
            if(hitEntity->hasComponent<HealthComponent>()){
                HealthComponent* healthCmp      = &EM.getComponent<HealthComponent>(*hitEntity);
                
                SoundComponent* enemSoundCmp    = &EM.getComponent<SoundComponent>(*hitEntity);
                SoundComponent* soundCmp        = &EM.getComponent<SoundComponent>(e);

                // Play sound
                enemSoundCmp->maskSounds[M_ZOMBIE_PAIN] |= SOUND_PLAY;
                soundCmp->maskSounds[M_BULLET_MEATIMPACT] |= SOUND_PLAY;
                

                // Check if has collided in vulnerable collider
                if(ray->colliderflags & COLLIDER_HEAD){
                    // Vulnerable collider, deals more damage
                    healthCmp->health -= damage * HEADSHOOT_MULTIPLIER;
                    enemyHit = 2;
                    
                }else {
                    // Normal collider, deals normal damage
                    healthCmp->health -= damage;
                    enemyHit = 1;
                }
            }

            if(moreKillPoints) {
                //Create particles on hit position
                createParticlesOnHit(e, ray, EM);

                // Mark head shot
                if(enemyHit == 2)   hitEntity->addTag<HeadShotMarkTag>();
                else                hitEntity->removeTag<HeadShotMarkTag>();
            }
        }
        else if(ray->bodyflags & BARREL_COLLISION){
            if(hitEntity->hasComponent<HealthComponent>()){
                HealthComponent* healthCmp  = &EM.getComponent<HealthComponent>(*hitEntity);
                WeaponComponent* wCmp       = &EM.getComponent<WeaponComponent>(e);
                // Normal collider, deals normal damage
                healthCmp->health -= wCmp->activeWeapon->damage;
                enemyHit = 1;
            }
        }
    }

    return enemyHit;
}

void WeaponSystem::changeWeapon(Entity& e, WeaponComponent* wCmp, bool secondWeapon, SoundComponent* sound) {
    // Check if the weapon is trying to get is the same it already has
    if      (secondWeapon   && wCmp->activeWeapon != wCmp->secondWeapon.get())    changeWeapon(e, wCmp, sound);
    else if (!secondWeapon  && wCmp->activeWeapon != wCmp->firstWeapon.get())     changeWeapon(e, wCmp, sound);

    wCmp->getFirstWeapon    = false;
    wCmp->getSecondWeapon   = false;
}

void WeaponSystem::changeWeapon(Entity& e, WeaponComponent* wCmp, SoundComponent* sound) {
    bool changeWeapon{false};
    uint16_t lastWeapon {wCmp->activeWeapon->weaponID};

    if(wCmp->activeWeapon == wCmp->firstWeapon.get()) {
        // Get the second weapon if it has two
        if(wCmp->secondWeapon->weaponID != 0) {
            wCmp->activeWeapon->weaponNode->setVisible(false);
            wCmp->activeWeapon  = wCmp->secondWeapon.get();
            changeWeapon        = true;
        }
    }
    else {
        // Get the first weapon
        wCmp->activeWeapon->weaponNode->setVisible(false);
        wCmp->activeWeapon  = wCmp->firstWeapon.get();
        changeWeapon        = true;
    }

    if(changeWeapon) {
        changeAnimation(e, wCmp);
        //Stop playing weapon sound
        sound->maskSounds[reloadSounds[lastWeapon-1]] |= SOUND_STOP;
        sound->maskSounds[M_WEAPON_SWITCH] |= SOUND_PLAY;

        // After changing weapon, reset values
        wCmp->changeWeaponTime  = CHANGE_WEAPON_TIME;
        wCmp->activeWeapon->weaponNode->setVisible(true);
        wCmp->currentRecoil     = 0.f;
        wCmp->maxRecoilAdded    = 0.f;

        if(wCmp->activeWeapon->reloadElapsed < wCmp->activeWeapon->reloadTime)
            wCmp->activeWeapon->weaponState |= WEAPON_RELOAD;
    }

    wCmp->changeWeapon = false;
}

void WeaponSystem::changeAnimation(Entity& e, WeaponComponent* wcomp){
    EntityFactory* eFactory = EntityFactory::Instance();
    eFactory->clearAnimations(e);
    bool isTShotgun{false};
    if(wcomp->activeWeapon->weaponID == 8){
        isTShotgun = true;
    }
    eFactory->createAnimation(e, static_cast<int>(RunningAnimationWeapon::IDLE_ANIMATION), WeaponTemplates::WEAPON_IDLE_ANIM[wcomp->activeWeapon->weaponID-1].c_str(), true, true, true);
    eFactory->createAnimation(e, static_cast<int>(RunningAnimationWeapon::SHOOT_ANIMATION), WeaponTemplates::WEAPON_SHOT_ANIM[wcomp->activeWeapon->weaponID-1].c_str(), false, true, false);
    eFactory->createAnimation(e, static_cast<int>(RunningAnimationWeapon::RELOAD_ANIMATION), WeaponTemplates::WEAPON_RELOAD_ANIM[wcomp->activeWeapon->weaponID-1].c_str(), isTShotgun, true, false);

    //Create special animations for weapons
    //Rifle
    if(wcomp->activeWeapon->weaponID == 6){
        eFactory->createAnimation(e, static_cast<int>(RunningAnimationRifle::LAST_SHOOT_ANIMATION), WeaponTemplates::RIFLE_LAST_SHOT_ANIM.c_str(), false, true, false);
    }else if(isTShotgun){
        //Tactical shotgun
        eFactory->createAnimation(e, static_cast<int>(RunningAnimationTactShootgun::FIRST_RELOAD_ANIMATION), WeaponTemplates::TSHOTGUN_FIRST_RELOAD_ANIM.c_str(), false, true, false);
        eFactory->createAnimation(e, static_cast<int>(RunningAnimationTactShootgun::LAST_RELOAD_ANIMATION), WeaponTemplates::TSHOTGUN_LAST_RELOAD_ANIM.c_str(), false, true, false);
    }
}

void WeaponSystem::reloadAmmo(EntityMan& EM, Entity& e) {
    WeaponComponent* wCmp       = &EM.getComponent<WeaponComponent>(e);
    SoundComponent* soundCmp    = &EM.getComponent<SoundComponent>(e);

    WeaponData* weapon = wCmp->activeWeapon;

    // If ammo is full return
    weapon->weaponState &= ~(WEAPON_SHOT | WEAPON_RELOAD);
    if (weapon->ammo >= weapon->magSize) return;
    // If ammo is 0 play no ammo sound and return
    if(weapon->totalAmmo == 0){
        soundCmp->maskSounds[M_DIALOGUES] |= SOUND_PLAY;
        soundCmp->paramValues.insert_or_assign("playerDialogue", PLAYER_DIALOGUE_PLAYING);
        soundCmp->paramValues.insert_or_assign("line", NO_AMMO_SOUND);
        return;
    }
    
    // Reset variables
    weapon->lastShot = weapon->fireRate;

    // Play reload sound
    soundCmp->maskSounds[reloadSounds[weapon->weaponID-1]] &= ~SOUND_PLAYING;
    soundCmp->maskSounds[reloadSounds[weapon->weaponID-1]] |= SOUND_PLAY;

    // Check if it is a combat shotgun (WeaponID = 8)
    if(weapon->weaponID == 8){
        // Calculate reload duration depending on the number of bullets
        int reload {};
        if(weapon->totalAmmo >= weapon->magSize - weapon->ammo) 
            reload = weapon->ammo;
        else 
            reload = weapon->magSize - weapon->totalAmmo;
        
        if(e.hasComponent<AnimationComponent>()){
            //Activate reload animation
            AnimationComponent* animC = &EM.getComponent<AnimationComponent>(e);
            if(animC != nullptr){
                animC->rAnimation = static_cast<int>(RunningAnimationTactShootgun::FIRST_RELOAD_ANIMATION);
                animC->animations[animC->rAnimation].m_CurrentTime= 0.0;
                int reloadAnim = static_cast<int>(RunningAnimationTactShootgun::RELOAD_ANIMATION);
                animC->animations[reloadAnim].auxData = reload;
            }
        }
        soundCmp->maskSounds[reloadSounds[weapon->weaponID-1]] |= SOUND_UPDATE_PARAM;
        soundCmp->paramValues.insert_or_assign("balasEscopeta", static_cast<int>(reload));
        weapon->reloadElapsed = reload * 0.8;

    }
    else{
        weapon->reloadElapsed   = 0.0;
        if(e.hasComponent<AnimationComponent>()){
            //Activate reload animation
            AnimationComponent* animC = &EM.getComponent<AnimationComponent>(e);
            if(animC != nullptr){
                animC->rAnimation = static_cast<int>(RunningAnimationWeapon::RELOAD_ANIMATION);
                animC->animations[animC->rAnimation].m_CurrentTime= 0.0;
            }
        }
    }
    
}