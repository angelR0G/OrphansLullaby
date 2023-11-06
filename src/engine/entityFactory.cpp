#include "entityFactory.hpp"
#include "entityFactory.tpp"
#include "../engine/graphic/engine.hpp"

//Player
using CPlayer = MetaP::Typelist<RenderComponent, MovementComponent, InputComponent, SoundComponent, CollisionComponent, CameraComponent, WeaponComponent, HealthComponent, AnimationComponent>;
using TPlayer = MetaP::Typelist<PlayerTag>;

//Normal enemy
using CNEnemy = MetaP::Typelist<RenderComponent, MovementComponent, SoundComponent, CollisionComponent, AIComponent, HealthComponent, NavigationComponent, AnimationComponent, AttackStatsComponent>;
using TNEnemy = MetaP::Typelist<EnemyTag, EnemyNormalTag>;

//Explosive enemy
using CEXEnemy = MetaP::Typelist<RenderComponent, MovementComponent, SoundComponent, CollisionComponent, AIComponent, HealthComponent, NavigationComponent, AnimationComponent, AttackStatsComponent>;
using TEXEnemy = MetaP::Typelist<EnemyTag, EnemyExplosiveTag>;

//Rugby enemy
using CREnemy = MetaP::Typelist<RenderComponent, MovementComponent, SoundComponent, CollisionComponent, AIComponent, HealthComponent, NavigationComponent, AttackStatsComponent, AnimationComponent>;
using TREnemy = MetaP::Typelist<EnemyTag, EnemyRugbyTag>;

//Thrower enemy
using CTWEnemy = MetaP::Typelist<RenderComponent, MovementComponent, SoundComponent, CollisionComponent, AIComponent, HealthComponent, NavigationComponent, AttackStatsComponent, AnimationComponent>;
using TTWEnemy = MetaP::Typelist<EnemyTag, EnemyThrowerTag>;

//Enemy attack
using CAttack = MetaP::Typelist<CollisionComponent, SoundComponent, AttackComponent>;
using TAttack = MetaP::Typelist<>;

//Projectile attack
using CPAttack = MetaP::Typelist<RenderComponent, CollisionComponent, SoundComponent, AttackComponent, MovementComponent>;
using TPAttack = MetaP::Typelist<>;

//Gas attack
using CGAttack = MetaP::Typelist<CollisionComponent, AttackComponent>;
using TGAttack = MetaP::Typelist<>;

//Map
using CMap = MetaP::Typelist<>;
using TMap = MetaP::Typelist<>;

// Object
using CObject = MetaP::Typelist<RenderComponent, CollisionComponent>;
using TObject = MetaP::Typelist<>;

// SoundEmitter
using CSoundEmitter = MetaP::Typelist<SoundComponent>;
using TSoundEmitter = MetaP::Typelist<>;

//Vending machine
using CVendingMachine = MetaP::Typelist<RenderComponent, CollisionComponent, TriggerComponent, SoundComponent>;
using TBulletMachine = MetaP::Typelist<BulletMachineTag>;
using TInjectionMachine = MetaP::Typelist<InjectionMachineTag>;
using TEffectMachine = MetaP::Typelist<EffectMachineTag>;
using TWeaponMachine = MetaP::Typelist<WeaponMachineTag>;

//Wall door
using CWallDoor = MetaP::Typelist<RenderComponent, CollisionComponent, TriggerComponent, SoundComponent>;
using TWallDoor = MetaP::Typelist<WallDoorTag>;

//Barrels
using CBarrel = MetaP::Typelist<RenderComponent, CollisionComponent, HealthComponent, SoundComponent, AIComponent, AnimationComponent>;
using TBarrel = MetaP::Typelist<>;

//Triggers
using CTrigger = MetaP::Typelist<TriggerComponent>;
using TTrigger = MetaP::Typelist<>;

//Object destroyable by enemies
using CEnemyObjective = MetaP::Typelist<RenderComponent, HealthComponent, CollisionComponent, TriggerComponent, AIComponent>;
using TEnemyObjective = MetaP::Typelist<ReparableTag>;

//Movable object destroyable by enemies
using CEnemyMovableObjective = MetaP::Typelist<RenderComponent, HealthComponent, TriggerComponent, SoundComponent, AIComponent, NavigationComponent, MovementComponent>;
using TEnemyMovableObjective = MetaP::Typelist<ReparableTag, MovableObjectiveTag>;

//Destroyable
using CDestroyable = MetaP::Typelist<RenderComponent, HealthComponent, SoundComponent>;
using TDestroyable = MetaP::Typelist<>;

#define ENEMY_MEDIA_PATH(file) "media/enemies/" file
#define WEAPON_MEDIA_PATH(file) "media/weapons/" file
#define MAP_MEDIA_PATH(file) "media/maps/map1/models_textures/" file
#define MAP_PROPS_MEDIA_PATH(file) "media/maps/map_props/" file

const std::vector<const char*> MODEL    =   {ENEMY_MEDIA_PATH("models/normal_caminandoModel.fbx"), 
                                            ENEMY_MEDIA_PATH("models/explosivo_andandoModel.fbx"),
                                            MAP_PROPS_MEDIA_PATH("models/MaquinaMunicion.obj"),
                                            MAP_PROPS_MEDIA_PATH("models/MaquinaInjec.obj"),
                                            MAP_PROPS_MEDIA_PATH("models/door_arcade.obj"),
                                            ENEMY_MEDIA_PATH("models/tanque_andandoModel.fbx"),
                                            MAP_PROPS_MEDIA_PATH("models/barril_explosivo_estaticoModel.fbx"),
                                            MAP_PROPS_MEDIA_PATH("models/barril_senyuelo_estaticoModel.fbx"),
                                            ENEMY_MEDIA_PATH("models/escupidor_andandoModel.fbx"),
                                            MAP_PROPS_MEDIA_PATH("models/zona_defender.obj"),
                                            MAP_PROPS_MEDIA_PATH("models/effectMachine.obj"),
                                            ENEMY_MEDIA_PATH("models/Escupitajo.obj"),//spit
                                            ENEMY_MEDIA_PATH("models/Golpe_Suelo.obj"),//slow
                                            WEAPON_MEDIA_PATH("models/rocket.obj"),//rocket
                                            MAP_PROPS_MEDIA_PATH("models/cajacomunicaciones.obj"),
                                            MAP_PROPS_MEDIA_PATH("models/geolocalizador.obj"),
                                            MAP_PROPS_MEDIA_PATH("models/m1k3.obj"),
                                            MAP_PROPS_MEDIA_PATH("models/zona_gps.obj")};

const std::vector<const char*> TEXTURE  =   {ENEMY_MEDIA_PATH("materials/grull.mtl"), 
                                            ENEMY_MEDIA_PATH("materials/grullexplosive.mtl"),
                                            MAP_PROPS_MEDIA_PATH("materials/MaquinaMunicion.mtl"),
                                            MAP_PROPS_MEDIA_PATH("materials/MaquinaInjec.mtl"),
                                            MAP_PROPS_MEDIA_PATH("materials/TexturaZombieDisco.png"),
                                            ENEMY_MEDIA_PATH("materials/grullrugby.mtl"),
                                            MAP_PROPS_MEDIA_PATH("materials/BarrilExplosivo.mtl"),
                                            MAP_PROPS_MEDIA_PATH("materials/BarrilSenyuelo.mtl"),
                                            ENEMY_MEDIA_PATH("materials/grullthrower.mtl"),
                                            MAP_PROPS_MEDIA_PATH("materials/zona_defender.mtl"),
                                            MAP_PROPS_MEDIA_PATH("materials/effectMachine.mtl"),
                                            ENEMY_MEDIA_PATH("materials/Escupitajo.mtl"),
                                            ENEMY_MEDIA_PATH("materials/Golpe_Suelo.mtl"),
                                            WEAPON_MEDIA_PATH("materials/rocket.mtl"),
                                            MAP_PROPS_MEDIA_PATH("materials/cajacomunicaciones.mtl"),
                                            MAP_PROPS_MEDIA_PATH("materials/geolocalizador.mtl"),
                                            MAP_PROPS_MEDIA_PATH("materials/m1k3.mtl"),
                                            MAP_PROPS_MEDIA_PATH("materials/zona_gps.mtl")};

EntityFactory* EntityFactory::Instance(){


    if(pEntityFactory.get() ==  0){
        pEntityFactory = std::unique_ptr<EntityFactory>(new EntityFactory());
    }

    return pEntityFactory.get();
}

//Constructor
EntityFactory::EntityFactory(){}

//Destructor
EntityFactory::~EntityFactory(){
    // for(uint8_t i= 0; i<nodePtr.size(); i++){
    //     delete nodePtr[i];
    // }
    //nodePtr.clear();
}

void EntityFactory::initialize(EntityMan& em, PhysicsEngine& phy, GraphicEngine& e3d){
    entity_man  = &em;
    phy_engine  = &phy;
    engine      = &e3d;
}

MeshNode EntityFactory::createMesh(uint16_t indexModel, uint16_t indexTexture){
    MeshNode model;
    model.createMeshNode(MODEL[indexModel], TEXTURE[indexTexture]);
    return model;
    //nodePtr.push_back(std::move(entity));
    //return nodePtr[nodePtr.size()-1].get();
}

MeshNode EntityFactory::createMesh(const char* m, const char* texture){
    MeshNode model;
    model.createMeshNode(m, texture);
    return model;
    //nodePtr.push_back(std::move(entity));
    //return nodePtr[nodePtr.size()-1].get();
}

Entity& EntityFactory::initPlayer(const Transform& transform){
    return createPlayer<CPlayer, TPlayer>(transform);
}

//Crete enemies

Entity& EntityFactory::initEnemyNormal(float spawnx, float spawny, float spawnz, uint8_t target_enemy_msk){
    return createEnemyNormal<CNEnemy, TNEnemy>(spawnx, spawny, spawnz, target_enemy_msk);
}

Entity& EntityFactory::initEnemyExplosive(float spawnx, float spawny, float spawnz){
    return createEnemyExplosive<CEXEnemy, TEXEnemy>(spawnx, spawny, spawnz);
}

Entity& EntityFactory::initEnemyRugby(float spawnx, float spawny, float spawnz){
    return createEnemyRugby<CREnemy, TREnemy>(spawnx, spawny, spawnz);
}

Entity& EntityFactory::initEnemyThrower(float spawnx, float spawny, float spawnz){
    return createEnemyThrower<CTWEnemy, TTWEnemy>(spawnx, spawny, spawnz);
}

Entity& EntityFactory::createObject(const Transform& meshTrans, const Transform& colTrans, uint8_t pathIndex, size_t modelIndex) {
    return createObject<CObject, TObject>(meshTrans, colTrans, pathIndex, modelIndex);
}

Entity& EntityFactory::createSoundEmitter(const Transform& transform, int soundId) {
    return createSoundEmitter<CSoundEmitter, TSoundEmitter>(transform, soundId);
}

//Crete vending machine
Entity& EntityFactory::createBulletMachine(const Transform& entityTransform, const Transform& triggerTransform){
    return createBulletMachine<CVendingMachine, TBulletMachine>(entityTransform, triggerTransform);
}

Entity& EntityFactory::createHealthMachine(const Transform& entityTransform, const Transform& triggerTransform){
    return createHealthMachine<CVendingMachine, TInjectionMachine>(entityTransform, triggerTransform);
}

Entity& EntityFactory::createEffectMachine(const Transform& entityTransform, const Transform& triggerTransform){
    return createEffectMachine<CVendingMachine, TEffectMachine>(entityTransform, triggerTransform);
}

Entity& EntityFactory::createWeaponMachine(WeaponData weapon, const Transform& entityTransform, const Transform& triggerTransform){
    return createWeaponMachine<CVendingMachine, TWeaponMachine>(weapon, entityTransform, triggerTransform);
}

//Create wall door
Entity& EntityFactory::createWallDoor(const Transform& entityTransform, const Transform& triggerTransform, int modelPosition){
    return createWallDoor<CWallDoor, TWallDoor>(entityTransform, triggerTransform, modelPosition);
}

Entity* EntityFactory::createEnemyAttack(BasicComponent* bc, std::vector<float> atparams, uint16_t attackMask){
    return createEnemyAttack<CAttack, TAttack>(bc, atparams, attackMask);
}

Entity* EntityFactory::createProjectileAttack(BasicComponent* bc, std::vector<float> atparams, uint16_t attackMask, uint16_t collisionMask){
    return createProjectileAttack<CPAttack, TPAttack>(bc, atparams, attackMask, collisionMask);
}

Entity* EntityFactory::createExplosion(size_t source_id, BasicComponent* bc, std::vector<float> atparams, uint16_t attackMask){
    return createExplosion<CAttack, TAttack>(source_id, bc, atparams, attackMask);
}

Entity* EntityFactory::createBarrel(uint8_t barrel_type, const Transform& barrel_transform){
    return createBarrel<CBarrel, TBarrel>(barrel_type, barrel_transform);
}

void EntityFactory::initMap(uint8_t index){
    createMap<CMap, TMap>(index);
}

void EntityFactory::deleteEnemyNormal(Entity& e){
    deleteEnemyNormal<CNEnemy, TNEnemy>(e);
}

//Crete trigger entity
Entity& EntityFactory::createTrigger(const Transform& transform){
    return createTrigger<CTrigger, TTrigger>(transform);
}

//Crete destroyable electric panel entity
Entity& EntityFactory::createElectricPanel(const Transform& transform){
    return createElectricPanel<CDestroyable, TDestroyable>(transform);
}

Entity& EntityFactory::createNestEntity(const Transform& transform){
    return createNestEntity<CDestroyable, TDestroyable>(transform);
}

//Object destroyable by enemies
Entity& EntityFactory::createEnemyObjetive(const Transform& transform){
    return createEnemyObjetive<CEnemyObjective, TEnemyObjective>(transform);
}

//Object destroyable by enemies
Entity& EntityFactory::createMovableEnemyObjetive(const Transform& transform){
    return createMovableEnemyObjetive<CEnemyMovableObjective, TEnemyMovableObjective>(transform);
}

void EntityFactory::checkDeleteEnemy(Entity& e){

    if(e.template hasTag<EnemyTag>()){
        e.removeTag<MarkToDestroyTag>();
        e.addTag<MarkToDestroyDelayTag>();
    }

    // create explosive attack if the enemy to be deleted is explosive
    if(e.template hasTag<EnemyExplosiveTag>())
    {
        HealthComponent &hcmp   = entity_man->template getComponent<HealthComponent>(e);
        BasicComponent &bascmp  = entity_man->template getComponent<BasicComponent>(e);
        std::vector<float> atparams { 0, 5, 0, 30.0f, EXPLOSIVE_ENEMY_DAMAGE, hcmp.infection};

        RenderComponent *renCmp  = &entity_man->template getComponent<RenderComponent>(e);
        // //Remove blood emitter
        // if(renCmp->emitter.getEmitter() != nullptr)
        //     GraphicEngine::Instance()->removeParticleEmitter(renCmp->emitter.getEmitter(), false);

        //Create explosive emitter
        renCmp->emitter.createParticleEmitter(ParticleEmitterType::InfectionExplosionParticle);
        renCmp->emitter.setActiveForTime(0.2);

        //Create attack
        EntityFactory* factory = EntityFactory::Instance();
        Entity* explosion = factory->createExplosion(e.getId(), &bascmp, atparams, (PLAYER_COLLISION | ENEMY_COLLISION));
        SoundComponent *scE = &entity_man->getComponent<SoundComponent>(*explosion);
        scE->idEvent.emplace_back(ZOMBIE_EXPLOSION);
        scE->maskSounds.emplace_back(SOUND_3D);
        scE->maskSounds[M_ZOMBIE_EXPLOSION] |= SOUND_PLAY;
    }
}


//Creates an animation for an entity
void EntityFactory::createAnimation(Entity& entity, int animType, std::string path, bool loop, bool isPlayer, bool active){
    if(entity.hasComponent<RenderComponent>() && entity.hasComponent<AnimationComponent>()){
        AnimationComponent *animComp = &entity_man->template getComponent<AnimationComponent>(entity);
        AnimationResource* anim{nullptr};
        if(isPlayer){
            WeaponComponent &wcomp = entity_man->template getComponent<WeaponComponent>(entity);
            wcomp.activeWeapon->weaponNode->createAnimation(active, path);
            anim = wcomp.activeWeapon->weaponNode->getAnimation();
        }else{
            RenderComponent &rcomp = entity_man->template getComponent<RenderComponent>(entity);
            rcomp.node.createAnimation(active, path);
            anim = rcomp.node.getAnimation();
        }
        std::array<glm::mat4, MAX_BONES> mBones{};
        for (int i = 0; i < MAX_BONES; i++)
			mBones[i] = glm::mat4(1.0f);
        animComp->animations.push_back(Animation{loop, anim->getAnimationDuration(), anim->getAnimationTickPerSecond(), anim, mBones});
        if(active){
            animComp->rAnimation = animComp->animations.size()-1;
            if(loop) animComp->prevAnimation = animComp->animations.size()-1;
        }
        
    }
}

void EntityFactory::clearAnimations(Entity& entity){
    if(entity.hasComponent<RenderComponent>() && entity.hasComponent<AnimationComponent>()){
        AnimationComponent *animComp = &entity_man->template getComponent<AnimationComponent>(entity);
        animComp->animations.clear();
    }
}

void EntityFactory::preloadWeaponsAndEnemies() {
    // Load enemies meshes
    engine->loadSceneMesh(MODEL[MODEL_ENEMY_NORMAL]);
    engine->loadSceneMesh(MODEL[MODEL_ENEMY_EXPLOSIVE]);
    engine->loadSceneMesh(MODEL[MODEL_ENEMY_THROWER]);
    engine->loadSceneMesh(MODEL[MODEL_ENEMY_RUGBY]);
    engine->loadSceneMesh(MODEL[MODEL_SPIT_ATTACK]);
    engine->loadSceneMesh(MODEL[MODEL_SLOW_ATTACK]);
    //engine->loadSceneMesh(MODEL[MODEL_ROCKET_ATTACK]);

    // Load enemies materials
    engine->loadMaterial(MODEL[TEXTURE_ENEMY_NORMAL]);
    engine->loadMaterial(MODEL[TEXTURE_ENEMY_EXPLOSIVE]);
    engine->loadMaterial(MODEL[TEXTURE_ENEMY_THROWER]);
    engine->loadMaterial(MODEL[TEXTURE_ENEMY_RUGBY]);
    engine->loadMaterial(MODEL[TEXTURE_SPIT_ATTACK]);
    engine->loadMaterial(MODEL[TEXTURE_SLOW_ATTACK]);

    engine->loadAnimation(WALK_ANIMATIONS[ANIMATION_ENEMY_NORMAL_WALK], MODEL[MODEL_ENEMY_NORMAL]);
    engine->loadAnimation(WALK_ANIMATIONS[ANIMATION_ENEMY_EXPLOSIVE_WALK], MODEL[MODEL_ENEMY_EXPLOSIVE]);
    engine->loadAnimation(WALK_ANIMATIONS[ANIMATION_ENEMY_THROWER_WALK], MODEL[MODEL_ENEMY_THROWER]);
    engine->loadAnimation(WALK_ANIMATIONS[ANIMATION_ENEMY_TANK_WALK], MODEL[MODEL_ENEMY_RUGBY]);

    engine->loadAnimation(ATTACK_ANIMATIONS[ANIMATION_ENEMY_NORMAL_ATTACK], MODEL[MODEL_ENEMY_NORMAL]);
    engine->loadAnimation(ATTACK_ANIMATIONS[ANIMATION_ENEMY_EXPLOSIVE_ATTACK], MODEL[MODEL_ENEMY_EXPLOSIVE]);
    engine->loadAnimation(ATTACK_ANIMATIONS[ANIMATION_ENEMY_THROWER_ATTACK], MODEL[MODEL_ENEMY_THROWER]);
    engine->loadAnimation(ATTACK_ANIMATIONS[ANIMATION_ENEMY_TANK_ATTACK], MODEL[MODEL_ENEMY_RUGBY]);
    engine->loadAnimation(ATTACK_ANIMATIONS[ANIMATION_ENEMY_THROWER_SPIT_ATTACK], MODEL[MODEL_ENEMY_THROWER]);
    engine->loadAnimation(ATTACK_ANIMATIONS[ANIMATION_ENEMY_TANK_SLOW_ATTACK], MODEL[MODEL_ENEMY_RUGBY]);

    engine->loadAnimation(DEATH_ANIMATIONS[ANIMATION_ENEMY_NORMAL_WALK], MODEL[MODEL_ENEMY_NORMAL]);
    engine->loadAnimation(DEATH_ANIMATIONS[ANIMATION_ENEMY_EXPLOSIVE_WALK], MODEL[MODEL_ENEMY_EXPLOSIVE]);
    engine->loadAnimation(DEATH_ANIMATIONS[ANIMATION_ENEMY_THROWER_WALK], MODEL[MODEL_ENEMY_THROWER]);
    engine->loadAnimation(DEATH_ANIMATIONS[ANIMATION_ENEMY_TANK_WALK], MODEL[MODEL_ENEMY_RUGBY]);
    //engine->loadMaterial(MODEL[TEXTURE_ROCKET_ATTACK]);

    // Load weapon meshes and materials
    for(size_t i{0}; i<WeaponTemplates::WEAPON_MODEL.size(); ++i) {
        engine->loadSceneMesh(WeaponTemplates::WEAPON_MODEL[i]);
        engine->loadMaterial(WeaponTemplates::WEAPON_TEXTURE[i]);
        engine->loadAnimation(WeaponTemplates::WEAPON_IDLE_ANIM[i], WeaponTemplates::WEAPON_MODEL[i]);
        engine->loadAnimation(WeaponTemplates::WEAPON_SHOT_ANIM[i], WeaponTemplates::WEAPON_MODEL[i]);
        engine->loadAnimation(WeaponTemplates::WEAPON_RELOAD_ANIM[i], WeaponTemplates::WEAPON_MODEL[i]);
        if(i==5)
            engine->loadAnimation(WeaponTemplates::RIFLE_LAST_SHOT_ANIM, WeaponTemplates::WEAPON_MODEL[i]);
        else if(i == 7){
            engine->loadAnimation(WeaponTemplates::TSHOTGUN_FIRST_RELOAD_ANIM, WeaponTemplates::WEAPON_MODEL[i]);
            engine->loadAnimation(WeaponTemplates::TSHOTGUN_LAST_RELOAD_ANIM, WeaponTemplates::WEAPON_MODEL[i]);
        }
    }
}