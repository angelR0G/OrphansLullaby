#include "collisionCallback.hpp"

#include "../entitymanager.hpp"
#include "../types.hpp"
#include "../../utils/collisionTypes.hpp"
#include "../../manager/gamemanager.hpp"
#include "../../manager/eventmanager.hpp"
#include "../../Fmod/MasksPositions.hpp"
#include "../entityFactory.hpp"
#include "../graphic/engine.hpp"
#include "../../utils/WeaponTemplates.hpp"

#define COST_AMMO           300
#define COST_INJECTION      350
#define COST_REPAIR         50
#define COST_EFFECT         200
#define COST_UPGRADE_BASE   600
#define COST_UPGRADE_INC    300

#define UI_TEXT_SIZE    0.35
#define UI_HOffset      200
using wData = std::pair<int, std::string>;
const static std::vector<wData> COST_WEAPON{
    {50,    "Pistola"}, //Pistol
    {120,   "Revolver"}, //Revolver
    {100,   "Pistola automática"}, //Automatic pistol
    {160,   "Subfusil"}, //Submachinegun
    {220,   "Rifle de asalto"}, //Assault rifle
    {180,   "Rifle"}, //rifle
    {220,   "Arma de rayos"}, //raygun
    {220,   "Escopeta táctica"}, //Combat shotgun
    {180,   "Escopeta"}, //shotgun
    {240,   "Ametralladora"}, //Machine gun
    {300,   "Lanzacohetes"}};//Bazooka

const static std::vector<int> COST_DOOR{
    50,     //Entrance
    100,    //Electronics
    250,    //Jewelry
    100,    //Arcade (inside)
    200,    //Clothing
    350,    //Music
    200,    //Arcade
};

#define TRIGGER_TEXT_DOOR           0
#define TRIGGER_TEXT_AMMO           1
#define TRIGGER_TEXT_INJE           2
#define TRIGGER_TEXT_WEAPON         3
#define TRIGGER_TEXT_EFFECT         4
#define TRIGGER_TEXT_REPAIR         5
#define TRIGGER_TEXT_GOAL           6
#define TRIGGER_TEXT_UPGRADE        7
#define TRIGGER_TEXT_MAX_UPGRADE    8
#define TRIGGER_TEXT_REPAIR_M1KE    9

const static std::vector<std::string> TRIGGER_TEXTS{ 
        "Pulsa E para comprar la puerta [Coste: %]",
        "Pulsa E para comprar munición [Coste: " + std::to_string(COST_AMMO) + "]",
        "Pulsa E para comprar una inyección [Coste: " + std::to_string(COST_INJECTION) + "]",
        "Pulsa E para comprar % [Coste: %]",
        "Pulsa E para comprar un efecto aleatorio [Coste: " + std::to_string(COST_EFFECT) + "]",
        "Pulsa E para reparar la máquina [Coste: " + std::to_string(COST_REPAIR) + "]",
        "Pulsa E para interactuar",
        "Ya tienes este arma. Mejorar % [Coste: %]",
        "Ya tienes este arma mejorada al máximo. A repartir caña.",
        "Pulsa E para reparar a M1K3 [Coste: " + std::to_string(COST_REPAIR) + "]"
};

void triggerCallback(const reactphysics3d::CollisionCallback::CallbackData& data, EntityMan* EM, Entity* trigger, Entity* e);

void stopMovement(BasicComponent* basicCmp, MovementComponent* movCmp, CollisionDetect* colDet){
    
    if(abs(colDet->hitNY) > 0.6){
        basicCmp->y -= colDet->depth * colDet->hitNY;
    }else{
        //Slope
        basicCmp->x -= colDet->depth * colDet->hitNX;
        basicCmp->y += colDet->depth * colDet->hitNY;
        basicCmp->z -= colDet->depth * colDet->hitNZ;
    }

    if(colDet->depth * colDet->hitNY < 0)
        movCmp->vY = 0;
}

void undoMovement(BasicComponent* basicCmp, MovementComponent* movCmp, CollisionDetect* colDet, float depthTreshold){
    if(colDet->depth >= depthTreshold) {
        // Undo entity movement
        basicCmp->x = basicCmp->prevx;
        basicCmp->z = basicCmp->prevz;

        // Reset entity speed
        movCmp->vLinear = 0;
    }
}

void separateEntities(BasicComponent* basicCmp, BasicComponent* basicCmp2) {
    float dirX = (basicCmp2->x - basicCmp->x)/8.f;
    float dirY = (basicCmp2->y - basicCmp->y)/8.f;
    float dirZ = (basicCmp2->z - basicCmp->z)/8.f;

    // Move first entity
    basicCmp->x -= dirX;
    basicCmp->z -= dirZ;

    // Move second entity
    basicCmp2->x += dirX;
    basicCmp2->z += dirZ;
}

bool canAttackDamage(AttackComponent* aCmp, HealthComponent* hCmp) {
    bool attack {true};

    // Check all entities damaged by the attack
    for(size_t i=0; i<aCmp->damagedHealthId.size() && attack; ++i) {
        // If has been attack previously, cannot receive damage
        if(aCmp->damagedHealthId[i] == hCmp->healthId) attack = false;
    }

    return attack;
}

void CollisionDetect::onContact(const reactphysics3d::CollisionCallback::CallbackData& data){
    // Get number of collisions
    auto numCol = data.getNbContactPairs();

    EntityMan* EM = &GameManager::Instance()->EM;

    // Check every collision
    for(size_t i{0}; i<numCol; ++i) {
        CollisionCallback::ContactPair pair     = data.getContactPair(i);
        CollisionCallback::ContactPoint point   = pair.getContactPoint(0);
        reactphysics3d::Vector3 normal          = point.getWorldNormal();
        reactphysics3d::decimal d               = point.getPenetrationDepth();

        //Store values
        hitNX = normal.x;
        hitNY = normal.y;
        hitNZ = normal.z;
        depth = d;

        // Get colliding entities
        CollisionObjectData* objData1 = static_cast<CollisionObjectData*>(pair.getBody1()->getUserData());
        Entity* e1      = static_cast<Entity*>(objData1->entity);
        uint16_t flags1 = objData1->flags;

        CollisionObjectData* objData2 = static_cast<CollisionObjectData*>(pair.getBody2()->getUserData());
        Entity* e2      = static_cast<Entity*>(objData2->entity);
        uint16_t flags2 = objData2->flags;

        // Update entities depending on flags
        // Collision between same type entities
        // Enemies colliding
        if(flags1 & flags2 & ENEMY_COLLISION){
            BasicComponent* basicCmp1 = &EM->getComponent<BasicComponent>(*e1);
            BasicComponent* basicCmp2 = &EM->getComponent<BasicComponent>(*e2);
            separateEntities(basicCmp1, basicCmp2);
        }
        else {
            // Collision between different type entities
            bool done{false};
            do {
                // Player colliding with enemies and entities with solid objects
                if((flags1 & (PLAYER_COLLISION | ENEMY_COLLISION | PROJECTILE_SLOW_COLLISION)) && (flags2 & (NO_TRASPASABLE_COLLISION))){
                    BasicComponent* basicCmp1 = &EM->getComponent<BasicComponent>(*e1);
                    MovementComponent* movCmp    = &EM->getComponent<MovementComponent>(*e1);
                    stopMovement(basicCmp1, movCmp, this);
                }
                //Collisions with enemy attacks
                else if((flags1 & (PLAYER_COLLISION | ENEMY_COLLISION | DESTROYABLE_COLLISION)) && (flags2 & (ENEMIES_ATTACKS_COLLISION))){   
                    AttackComponent* acmp = &EM->getComponent<AttackComponent>(*e2);

                    // Check if entity can be damaged by this attack
                    if(acmp->damageTo & flags1) {
                        HealthComponent* hcmp = &EM->getComponent<HealthComponent>(*e1);

                        // Check if has been previously attacked
                        if(canAttackDamage(acmp, hcmp)) {
                            BasicComponent* basicCmp1 = &EM->getComponent<BasicComponent>(*e1);

                            if(flags1 == PLAYER_COLLISION) {
                                SoundComponent *playerSounds = &EM->getComponent<SoundComponent>(*e1);
                                // If is a slowing attack, apply effect
                                if(flags2 & PROJECTILE_SLOW_COLLISION){
                                    basicCmp1->slowEffect = SLOW_DURATION;
                                }

                                // Check if has infection resistance
                                int infectionValue1 = trunc(hcmp->infection);
                                if(!hasSpecialEffect(basicCmp1->spEffects, SpecialEffect::Inmunity))
                                    // The player get infection
                                    hcmp->infection += acmp->infection;
                                    int infectionValue2 = trunc(hcmp->infection);
                                    //if player reachs a new goal of infection play sound
                                    if(infectionValue2 > infectionValue1){
                                        playerSounds->maskSounds[M_AGONIC_BREATH] |= SOUND_PLAY;
                                    }
                                basicCmp1->effectsHealthLost += acmp->infection;
                                

                                // Play pain sound
                                playerSounds->maskSounds[M_PLAYER_PAIN] |= SOUND_PLAY;

                                BasicComponent* basicCmp2 = &EM->getComponent<BasicComponent>(*e2);

                                // Event
                                std::shared_ptr<Event> event ( new Event(EVENT_PLAYER_DAMAGED) );
                                event->dataFloat    = basicCmp2->x;
                                event->dataFloat2   = basicCmp2->z;
                                EventManager::Instance()->scheduleEvent(event);
                            }

                            // Check if has damage resistance and deal damage
                            if(hasSpecialEffect(basicCmp1->spEffects, SpecialEffect::Tank))
                                hcmp->health    -= acmp->damage * SP_EFFECT_DAMAGE_REDUCTION_VALUE;
                            else
                                hcmp->health    -= acmp->damage;

                            acmp->damagedHealthId.emplace_back(hcmp->healthId);
                        }
                    }
                    if(flags2 & PROJECTILE_SPIT_COLLISION && flags1 & PLAYER_COLLISION){
                        acmp->currentAttackTime = acmp->maxDurationAttack;
                    }
                }
                // Player colliding with enemies
                else if((flags1 & PLAYER_COLLISION) && (flags2 & ENEMY_COLLISION)){
                    // Undo enemy movement
                    BasicComponent* basicCmp2 = &EM->getComponent<BasicComponent>(*e2);
                    MovementComponent* movCmp2 = &EM->getComponent<MovementComponent>(*e2);
                    undoMovement(basicCmp2, movCmp2, this, 0.f);

                    // Undo player movement
                    BasicComponent* basicCmp1 = &EM->getComponent<BasicComponent>(*e1);
                    MovementComponent* movCmp1 = &EM->getComponent<MovementComponent>(*e1);
                    undoMovement(basicCmp1, movCmp2, this, 0.9f);
                }
                // Entity colliding with a trigger
                else if(flags1 & TRIGGER_COLLISION) {
                    if(e1->hasComponent<TriggerComponent>())
                        triggerCallback(data, EM, e1, e2);
                }
                // Movable destroyable colliding with solid objects
                else if((flags1 & DESTROYABLE_COLLISION) && (flags2 & SOLID_COLLISION)) {
                    if(e1->hasComponent<MovementComponent>()) {
                        BasicComponent* basicCmp1 = &EM->getComponent<BasicComponent>(*e1);
                        MovementComponent* movCmp    = &EM->getComponent<MovementComponent>(*e1);
                        stopMovement(basicCmp1, movCmp, this);
                    }
                }
                // Rocket and spit attack collision
                else if(flags1 & (PROJECTILE_ROCKET_COLLISION | PROJECTILE_SPIT_COLLISION) && !(flags2 & (TRIGGER_COLLISION | PLAYER_COLLISION))) {
                    AttackComponent* aCmp   = &EM->getComponent<AttackComponent>(*e1);
                    aCmp->currentAttackTime = aCmp->maxDurationAttack;
                }

                // Update variables
                done = !done;

                e1      = static_cast<Entity*>(objData2->entity);
                flags1  = objData2->flags;

                e2      = static_cast<Entity*>(objData1->entity);
                flags2  = objData1->flags;
            }while(done);
        }

    }
}

void triggerCallback(const reactphysics3d::CollisionCallback::CallbackData& data, EntityMan* EM, Entity* trigger, Entity* e) {
    TriggerComponent* triggerCmp    = &EM->getComponent<TriggerComponent>(*trigger);
    BasicComponent* basicCmp        = &EM->getComponent<BasicComponent>(*trigger);

    // If trigger is disabled, return
    if(!triggerCmp->bActive) return;

    // Increase number of entities inside trigger
    triggerCmp->inside++;

    // Check if player has entered
    if(e->hasTag<PlayerTag>()) {
        // Mark player is inside trigger
        triggerCmp->playerInside = true;

        GameManager* gameMan = GameManager::Instance();
        InputComponent*   inputCmp = &EM->getComponent<InputComponent>(*e);
        SoundComponent*   sC = &EM->getComponent<SoundComponent>(*e);
        if(trigger->hasTag<BulletMachineTag>()){
            SoundComponent* soundCmp = &EM->getComponent<SoundComponent>(*trigger);
            soundCmp->maskSounds[M_AMMO_MACHINE_GREETINGS] |= SOUND_PLAY;
        }
        else if(trigger->hasTag<InjectionMachineTag>()){
            SoundComponent* soundCmp = &EM->getComponent<SoundComponent>(*trigger);
            soundCmp->maskSounds[M_HEAL_MACHINE_GREETINGS] |= SOUND_PLAY;
        }

        //Check if interact with bullet machine
        if(trigger->hasTag<BulletMachineTag>()){
            gameMan->setTextCenterIMGUI(TRIGGER_TEXTS[TRIGGER_TEXT_AMMO], std::vector<float>{1.0f,1.0f,1.0f,1.0f}, 0, UI_HOffset, UI_TEXT_SIZE);
            //Check if the player has enought points
            if(inputCmp->interact){
                if(gameMan->checkEnoughtPoints(COST_AMMO)){
                    WeaponComponent*   wCmp = &EM->getComponent<WeaponComponent>(*e);
                    if(!(wCmp->activeWeapon->weaponState & INFINITE_WEAPON) 
                        && wCmp->activeWeapon->totalAmmo != wCmp->activeWeapon->totalSize){

                        inputCmp->interact = false;
                        gameMan->increasePoints((-1)*COST_AMMO);
                        wCmp->activeWeapon->totalAmmo = wCmp->activeWeapon->totalSize;

                        //Play buy sound
                        SoundComponent* soundCmp = &EM->getComponent<SoundComponent>(*trigger);
                        soundCmp->maskSounds[M_AMMO_MACHINE_EXIT] |= SOUND_PLAY;
                    }
                }
                //If the player doesnt have enought point play a sound.
                else if(inputCmp->interact){
                    sC->maskSounds[M_DIALOGUES] |= SOUND_PLAY;
                    sC->paramValues.insert_or_assign("playerDialogue", PLAYER_DIALOGUE_PLAYING);
                    sC->paramValues.insert_or_assign("line", NO_POINTS_SOUND); 
                }
            }
        }else if(trigger->hasTag<InjectionMachineTag>()){
            //Check if interact with injections machine
            gameMan->setTextCenterIMGUI(TRIGGER_TEXTS[TRIGGER_TEXT_INJE], std::vector<float>{1.0f,1.0f,1.0f,1.0f}, 0, UI_HOffset, UI_TEXT_SIZE);

            HealthComponent*   hCmp = &EM->getComponent<HealthComponent>(*e);
            if(hCmp->infection != 0){
                if(inputCmp->interact ){
                    if(gameMan->checkEnoughtPoints(COST_INJECTION)){
                        inputCmp->interact = false;
                        gameMan->increasePoints((-1)*COST_INJECTION);
                        hCmp->infection = 0.0;

                        SoundComponent* soundCmp = &EM->getComponent<SoundComponent>(*trigger);
                        soundCmp->maskSounds[M_HEAL_MACHINE_EXIT] |= SOUND_PLAY;
                    }
                    //If the player doesnt have enought point play a sound.
                    else if(inputCmp->interact){
                        sC->maskSounds[M_DIALOGUES] |= SOUND_PLAY;
                        sC->paramValues.insert_or_assign("playerDialogue", PLAYER_DIALOGUE_PLAYING);
                        sC->paramValues.insert_or_assign("line", NO_POINTS_SOUND); 
                    }
                }
            }
            
        }else if(trigger->hasTag<EffectMachineTag>()){
            gameMan->setTextCenterIMGUI(TRIGGER_TEXTS[TRIGGER_TEXT_EFFECT], std::vector<float>{1.0f,1.0f,1.0f,1.0f}, 0, UI_HOffset, UI_TEXT_SIZE);

            if(inputCmp->interact){
                if(gameMan->checkEnoughtPoints(COST_EFFECT)){
                    BasicComponent*   basicCmp = &EM->getComponent<BasicComponent>(*e);
                    SoundComponent* soundCmp = &EM->getComponent<SoundComponent>(*trigger);
                    inputCmp->interact = false;
                    gameMan->increasePoints((-1)*COST_EFFECT);

                    // Add a random effect
                    int effectrandom {(std::rand() % TOTAL_EFFECTS_COUNT) + 1};
                    basicCmp->addSpEffect = SpecialEffect(effectrandom);
                }
                //If the player doesnt have enought points play a sound.
                else if(inputCmp->interact) {
                    sC->maskSounds[M_DIALOGUES] |= SOUND_PLAY;
                    sC->paramValues.insert_or_assign("playerDialogue", PLAYER_DIALOGUE_PLAYING);
                    sC->paramValues.insert_or_assign("line", NO_POINTS_SOUND);
                }
            }

        }else if(trigger->hasTag<WallDoorTag>()){
            //Check if interact with wall door
            std::string text = gameMan->formatText(TRIGGER_TEXTS[TRIGGER_TEXT_DOOR], std::vector<char>{'%'}, std::vector<std::string>{std::to_string(COST_DOOR[triggerCmp->position])});
            gameMan->setTextCenterIMGUI(text, std::vector<float>{1.0f,1.0f,1.0f,1.0f}, 0, UI_HOffset, UI_TEXT_SIZE);

            if(inputCmp->interact){
                if(gameMan->checkEnoughtPoints(COST_DOOR[triggerCmp->position])){
                    inputCmp->interact = false;
                    gameMan->increasePoints((-1)*COST_DOOR[triggerCmp->position]);
                    // Unlocks and delete door
                    gameMan->unlockDoor(trigger->getId());
                    trigger->addTag<MarkToDestroyTag>();
                }
                //If the player doesnt have enought point play a sound.
                else if(inputCmp->interact){
                    sC->maskSounds[M_DIALOGUES] |= SOUND_PLAY;
                    sC->paramValues.insert_or_assign("playerDialogue", PLAYER_DIALOGUE_PLAYING);
                    sC->paramValues.insert_or_assign("line", NO_POINTS_SOUND); 
                }
            }
        }else if(trigger->hasTag<WeaponMachineTag>()){
            WeaponComponent&   wcomp = EM->getComponent<WeaponComponent>(*e);
            if(triggerCmp->weaponData.weaponID != wcomp.firstWeapon->weaponID && triggerCmp->weaponData.weaponID != wcomp.secondWeapon->weaponID){
                //Doesnt have the weapon
                std::string text = gameMan->formatText(TRIGGER_TEXTS[TRIGGER_TEXT_WEAPON], {'%', '%'}, {COST_WEAPON[triggerCmp->weaponData.weaponID-1].second, std::to_string(COST_WEAPON[triggerCmp->weaponData.weaponID-1].first)});
                gameMan->setTextCenterIMGUI(text, std::vector<float>{1.0f,1.0f,1.0f,1.0f}, 0, UI_HOffset, UI_TEXT_SIZE);

                if(inputCmp->interact) {
                    // Check if can buy the weapon
                    if(gameMan->checkEnoughtPoints(COST_WEAPON[triggerCmp->weaponData.weaponID-1].first)){
                
                        inputCmp->interact = false;
                        EntityFactory* entityFac    = EntityFactory::Instance();
                        gameMan->increasePoints((-1)*COST_WEAPON[triggerCmp->weaponData.weaponID-1].first);

                        sC->maskSounds[M_BUY] |= SOUND_PLAY;
                        sC->maskSounds[M_GET_OBJECT] |= SOUND_PLAY;
                        sC->maskSounds[M_DIALOGUES]   |= SOUND_PLAY;
                        sC->paramValues.insert_or_assign("playerDialogue", PLAYER_DIALOGUE_PLAYING);
                        sC->paramValues.insert_or_assign("line", BUY_WEAPON_SOUND); 

                        if(triggerCmp->weaponData.weaponState & INFINITE_WEAPON){  
                            //Remove the current weapon mesh
                            GraphicEngine::Instance()->removeSceneMesh(wcomp.firstWeapon->weaponNode->getMesh());
                            *wcomp.firstWeapon = triggerCmp->weaponData;
                            wcomp.firstWeapon->weaponNode = entityFac->createMesh(WeaponTemplates::WEAPON_MODEL[wcomp.firstWeapon->weaponID-1].c_str(), 
                                                                    WeaponTemplates::WEAPON_TEXTURE[wcomp.firstWeapon->weaponID-1].c_str());
                            wcomp.firstWeapon->weaponNode->setCastShadows(false);

                            // Change to that weapon
                            if(wcomp.activeWeapon->weaponID != wcomp.firstWeapon->weaponID) {
                                wcomp.changeWeapon = true;
                            }
                        }else{
                            //Remove the current weapon mesh
                            if(wcomp.secondWeapon->weaponID != 0)
                                GraphicEngine::Instance()->removeSceneMesh(wcomp.secondWeapon->weaponNode->getMesh());
                            *wcomp.secondWeapon = triggerCmp->weaponData;
                            wcomp.secondWeapon->weaponNode = entityFac->createMesh(WeaponTemplates::WEAPON_MODEL[wcomp.secondWeapon->weaponID-1].c_str(), 
                                                                        WeaponTemplates::WEAPON_TEXTURE[wcomp.secondWeapon->weaponID-1].c_str());
                            wcomp.secondWeapon->weaponNode->setCastShadows(false);
                            
                            // Change to that weapon
                            if(wcomp.activeWeapon->weaponID != wcomp.secondWeapon->weaponID) {
                                wcomp.changeWeapon = true;
                            }
                        }
                        EntityFactory* eFactory = EntityFactory::Instance();
                        eFactory->clearAnimations(*e);
                        bool isTShotgun{false};
                        if(wcomp.activeWeapon->weaponID == 8){
                            isTShotgun = true;
                        }
                        eFactory->createAnimation(*e, static_cast<int>(RunningAnimationWeapon::IDLE_ANIMATION), WeaponTemplates::WEAPON_IDLE_ANIM[wcomp.activeWeapon->weaponID-1].c_str(), true, true, true);
                        eFactory->createAnimation(*e, static_cast<int>(RunningAnimationWeapon::SHOOT_ANIMATION), WeaponTemplates::WEAPON_SHOT_ANIM[wcomp.activeWeapon->weaponID-1].c_str(), false, true, false);
                        eFactory->createAnimation(*e, static_cast<int>(RunningAnimationWeapon::RELOAD_ANIMATION), WeaponTemplates::WEAPON_RELOAD_ANIM[wcomp.activeWeapon->weaponID-1].c_str(), isTShotgun, true, false);

                        //Create special animations for weapons
                        //Rifle
                        if(wcomp.activeWeapon->weaponID == 6){
                            eFactory->createAnimation(*e, static_cast<int>(RunningAnimationRifle::LAST_SHOOT_ANIMATION), WeaponTemplates::RIFLE_LAST_SHOT_ANIM.c_str(), false, true, false);
                        }else if(isTShotgun){
                            //Tactical shotgun
                            eFactory->createAnimation(*e, static_cast<int>(RunningAnimationTactShootgun::FIRST_RELOAD_ANIMATION), WeaponTemplates::TSHOTGUN_FIRST_RELOAD_ANIM.c_str(), false, true, false);
                            eFactory->createAnimation(*e, static_cast<int>(RunningAnimationTactShootgun::LAST_RELOAD_ANIMATION), WeaponTemplates::TSHOTGUN_LAST_RELOAD_ANIM.c_str(), false, true, false);
                        }
                        
                        // If has steady aim powerup, change recoil values
                        BasicComponent*   basicCmp = &EM->getComponent<BasicComponent>(*e);
                        if(hasSpecialEffect(basicCmp->spEffects, SpecialEffect::Steady_Aim)) {
                            wcomp.activeWeapon->recoilForce /= 2.0f;
                        }
                    }
                    //If the player doesnt have enought point play a sound.
                    else{
                        sC->maskSounds[M_DIALOGUES] |= SOUND_PLAY;
                        sC->paramValues.insert_or_assign("playerDialogue", PLAYER_DIALOGUE_PLAYING);
                        sC->paramValues.insert_or_assign("line", NO_POINTS_SOUND); 
                    }
                }
            }else{
                // Has the weapon, recover it from weapon component
                WeaponData* weaponRef {wcomp.firstWeapon.get()};
                if(wcomp.secondWeapon->weaponID == triggerCmp->weaponData.weaponID) 
                    weaponRef = wcomp.secondWeapon.get();

                // Check upgrades
                if(weaponRef->upgrades < MAX_WEAPON_LEVEL) {
                    // Weapon can be upgraded
                    int         upgradeCost {COST_UPGRADE_BASE + COST_UPGRADE_INC*weaponRef->upgrades};
                    std::string weaponName  {COST_WEAPON[triggerCmp->weaponData.weaponID-1].second};
                    std::string text = gameMan->formatText(TRIGGER_TEXTS[TRIGGER_TEXT_UPGRADE], {'%', '%'}, {weaponName, std::to_string(upgradeCost)});
                    gameMan->setTextCenterIMGUI(text, std::vector<float>{1.0f,1.0f,1.0f,1.0f}, 0, UI_HOffset, UI_TEXT_SIZE);

                    if(inputCmp->interact) {
                        // Check if has enough points to upgrade
                        if(gameMan->checkEnoughtPoints(upgradeCost)){
                            // Increase weapon level
                            weaponRef->upgrades++;
                            weaponRef->damage = weaponRef->baseDamage + weaponRef->baseDamage/2.f * weaponRef->upgrades;

                            //Play sound
                            sC->maskSounds[M_BUY] |= SOUND_PLAY;

                            // Reduce points
                            gameMan->increasePoints((-1)*upgradeCost);
                            inputCmp->interact = false;
                        }
                        //If the player doesnt have enought point play a sound.
                        else{
                            sC->maskSounds[M_DIALOGUES] |= SOUND_PLAY;
                            sC->paramValues.insert_or_assign("playerDialogue", PLAYER_DIALOGUE_PLAYING);
                            sC->paramValues.insert_or_assign("line", NO_POINTS_SOUND); 
                        }
                    }
                }
                else {
                    // Weapon is fully upgraded
                    gameMan->setTextCenterIMGUI(TRIGGER_TEXTS[TRIGGER_TEXT_MAX_UPGRADE], std::vector<float>{1.0f,1.0f,1.0f,1.0f}, 0, UI_HOffset, UI_TEXT_SIZE);
                }
            }

        }else if(trigger->hasTag<GoalInteractTag>()){
            gameMan->setTextCenterIMGUI(TRIGGER_TEXTS[TRIGGER_TEXT_GOAL], std::vector<float>{1.0f,1.0f,1.0f,1.0f}, 0, UI_HOffset, UI_TEXT_SIZE);
            
            //Check if interact with goal objects
            if(inputCmp->interact) {
                gameMan->completeGoal();

                trigger->addTag<MarkToDestroyTag>();

                gameMan->activeText(false);
            }
            
        }else if(trigger->hasTag<ReparableTag>() || trigger->hasTag<MovableObjectiveTag>()){
            //Check if interact with reparable objects
            HealthComponent*   hCmp = &EM->getComponent<HealthComponent>(*trigger);
            if(hCmp->health < 0.0001) {
                // Check if is not movable objective
                if(!trigger->hasTag<MovableObjectiveTag>()){
                    gameMan->setTextCenterIMGUI(TRIGGER_TEXTS[TRIGGER_TEXT_REPAIR], std::vector<float>{1.0f,1.0f,1.0f,1.0f}, 0, UI_HOffset, UI_TEXT_SIZE);
                }
                else{
                    gameMan->setTextCenterIMGUI(TRIGGER_TEXTS[TRIGGER_TEXT_REPAIR_M1KE], std::vector<float>{1.0f,1.0f,1.0f,1.0f}, 0, UI_HOffset, UI_TEXT_SIZE);
                }
                
                if(inputCmp->interact && gameMan->checkEnoughtPoints(COST_REPAIR)){
                    gameMan->activeText(false);
                    gameMan->increasePoints((-1)*COST_REPAIR);

                    // Restore health
                    hCmp->health = 100;
                    if(trigger->hasTag<M1K3Tag>()){
                        SoundComponent*   sCmp = &EM->getComponent<SoundComponent>(*trigger);
                        if(sCmp->paramValues.find("M1K3estado") != sCmp->paramValues.end()){
                            if(sCmp->paramValues.find("M1K3estado")->second !=1){
                                sCmp->paramValues.insert_or_assign("M1K3estado", 1);
                                sCmp->maskSounds[M_M1K3_FLY]          |=  SOUND_UPDATE_PARAM;
                            }
                        }
                    }

                    // Remove broken tag, if it has it
                    if(trigger->hasTag<BrokenObjectTag>()) trigger->removeTag<BrokenObjectTag>();
                }
            }   
        }
    }
}