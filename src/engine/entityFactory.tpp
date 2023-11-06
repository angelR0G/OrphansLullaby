#include "entityFactory.hpp"
#include "../Fmod/soundFactory.hpp"
#include "../utils/collisionTypes.hpp"
#include "../utils/transform.hpp"
#include "../utils/WeaponTemplates.hpp"

#include "../manager/gamemanager.hpp"
#include "../engine/graphic/engine.hpp"
#include "../engine/graphic/resource/animationresource.hpp"

#include "utils/bt/behaviourtree.hpp"
#include "utils/bt/nodesequence.hpp"
#include "utils/bt/nodeselector.hpp"
#include "utils/bt/checktargetYposition.hpp"
#include "utils/bt/checktargetvisible.hpp"
#include "utils/bt/actionfind.hpp"
#include "utils/bt/arrivetowaypoint.hpp"
#include "utils/bt/calculatewaypoint.hpp"
#include "utils/bt/checkattackdist.hpp"
#include "utils/bt/basicattack.hpp"
#include "utils/bt/specialattack.hpp"
#include "utils/bt/explosiveattack.hpp"
#include "utils/bt/checkexplosiveattack.hpp"
#include "utils/bt/checkslowattackcd.hpp"
#include "utils/bt/checkspitattack.hpp"
#include "utils/bt/actionflee.hpp"
#include "utils/bt/checkcurrenthealth.hpp"
#include "utils/bt/increasegoaltime.hpp"
#include "utils/bt/createexplosion.hpp"
#include "utils/bt/explosivebarrelcd.hpp"
#include "utils/bt/targetbarrelcd.hpp"
#include "utils/bt/checkroutepoint.hpp"
#include "utils/bt/checkdoorfront.hpp"
#include "utils/bt/checkplayernearby.hpp"

#include "../Fmod/MasksPositions.hpp"

#define PI 3.1415926
#define WEAPON_MEDIA_PATH(file) "media/weapons/" file
#define DOOR_MEDIA_PATH(file) doorsModelPath + file

const std::vector<std::string> mapFiles {
    "media/maps/map1/map_data/map.txt",
    "media/maps/map2/map_data/map.txt",
    "media/maps/menuscene/map_data/map.txt"
};

const std::vector<std::string> modelFiles {
    "media/maps/map1/models/",
    "media/maps/map2/models/",
    "media/maps/menuscene/models/"
};

const std::vector<std::string> textureFiles {
    "media/maps/map1/textures/",
    "media/maps/map2/textures/",
    "media/maps/menuscene/textures/"
};

const std::vector<std::string> materialFiles {
    "media/maps/map1/materials/",
    "media/maps/map2/materials/",
    "media/maps/menuscene/materials/"
};

const std::string mapProps {"media/maps/map_props/"};

#define ENEMY_MEDIA_PATH(file) "media/enemies/" file
#define MAP_PROPS_MEDIA_PATH(file) "media/maps/map_props/" file

const std::vector<const char*> WALK_ANIMATIONS = {  ENEMY_MEDIA_PATH("models/normal_caminando.fbx"),
                                                    ENEMY_MEDIA_PATH("models/explosivo_andando.fbx"),
                                                    ENEMY_MEDIA_PATH("models/escupidor_andando.fbx"),
                                                    ENEMY_MEDIA_PATH("models/tanque_andando.fbx")};

const std::vector<const char*>ATTACK_ANIMATIONS = { ENEMY_MEDIA_PATH("models/normal_atacando_1.fbx"),
                                                    ENEMY_MEDIA_PATH("models/explosivo_atacando.fbx"),
                                                    ENEMY_MEDIA_PATH("models/escupidor_atacando.fbx"),
                                                    ENEMY_MEDIA_PATH("models/tanque_atacando.fbx"),
                                                    ENEMY_MEDIA_PATH("models/escupidor_escupiendo.fbx"),
                                                    ENEMY_MEDIA_PATH("models/tanque_ataquesuelo.fbx")};

const std::vector<const char*>DEATH_ANIMATIONS = { ENEMY_MEDIA_PATH("models/normal_muriendo2_1.fbx"),
                                                    ENEMY_MEDIA_PATH("models/explosivo_muriendo2_2.fbx"),
                                                    ENEMY_MEDIA_PATH("models/escupidor_muerte2_2.fbx"),
                                                    ENEMY_MEDIA_PATH("models/tanque_muriendo.fbx")};

//Store all door models and materials used in the game
const std::string doorsModelPath {"media/maps/map_props/"};
const static std::vector<std::pair<std::string, std::string>> modelDoor{
    {DOOR_MEDIA_PATH("/models/Door_mall.obj"), DOOR_MEDIA_PATH("/materials/Door_mall.mtl")},
    {DOOR_MEDIA_PATH("/models/Door_shop.obj"), DOOR_MEDIA_PATH("/materials/Door_shop.mtl")},
    {DOOR_MEDIA_PATH("/models/Door_shop.obj"), DOOR_MEDIA_PATH("/materials/Door_shop.mtl")},
    {DOOR_MEDIA_PATH("/models/Door_woodenBarriers.obj"), DOOR_MEDIA_PATH("/materials/Door_woodenBarriers.mtl")},
    {DOOR_MEDIA_PATH("/models/Door_shop.obj"), DOOR_MEDIA_PATH("/materials/Door_shop.mtl")},
    {DOOR_MEDIA_PATH("/models/Door_shop.obj"), DOOR_MEDIA_PATH("/materials/Door_shop.mtl")},
    {DOOR_MEDIA_PATH("/models/Door_arcade.obj"), DOOR_MEDIA_PATH("/materials/Door_arcade.mtl")},
};

template <typename Comps, typename Tags>
Entity& EntityFactory::createPlayer(const Transform& transform)
{
    return createPlayer_imp(Comps{}, Tags{}, transform);
}

template <typename... Comps, typename... Tags>
Entity& EntityFactory::createPlayer_imp(MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>, const Transform& transform)
{

    Entity &entity = entity_man->createEntity(BasicComponent{.x = transform.x, .y = transform.y, .z = transform.z, .orientation = transform.ry*PI/180});

    ((void)entity_man->addComponent<Comps>(entity, Comps{}), ...);
    ((void)entity_man->addTag<Tags>(entity), ...);

    RenderComponent &rcomp = entity_man->template getComponent<RenderComponent>(entity);
    rcomp.emitter.createParticleEmitter(ParticleEmitterType::BloodParticle);
    rcomp.emitter.setActive(false);

    SoundComponent *s = &entity_man->getComponent<SoundComponent>(entity);
    SoundFactory* sFac = SoundFactory::Instance();

    s->idEvent.emplace_back(PISTOL_SHOOT);
    s->maskSounds.emplace_back(SOUND_DEFAULT);
    s->idEvent.emplace_back(PISTOL_RELOAD);
    s->maskSounds.emplace_back(SOUND_NOT_RELEASE);
    s->idInstance.emplace_back(sFac->createInstanceFromEvent(PISTOL_RELOAD, false, Transform{std::vector<float>{0,0,0,0,0,0,0,0,0}}));
    s->idEvent.emplace_back(HITMARKER);
    s->maskSounds.emplace_back(SOUND_DEFAULT);
    s->idEvent.emplace_back(BULLET_MEATIMPACT);
    s->maskSounds.emplace_back(SOUND_3D);
    s->idEvent.emplace_back(PLAYER_PAIN);
    s->maskSounds.emplace_back(SOUND_DEFAULT);
    s->idEvent.emplace_back(PLAYER_STEPS);
    s->maskSounds.emplace_back(SOUND_NOT_RELEASE);
    // AQUI ESTA DE MOMENTO EL SONIDO DE AMBIENTE
    s->idEvent.emplace_back(AMBIENT);
    s->maskSounds.emplace_back(SOUND_PLAY);
    // AQUI ESTA DE MOMENTO EL SONIDO DE AMBIENTE
    s->idEvent.emplace_back(DIALOGUES);
    s->maskSounds.emplace_back(SOUND_DIALOGUE);
    s->idInstance.emplace_back(sFac->createInstanceFromEvent(PLAYER_STEPS, false, Transform{std::vector<float>{0,0,0,0,0,0,0,0,0}}));
    s->paramValues.insert_or_assign("velocidad", 0);
    s->idInstance.emplace_back(sFac->createDialogue(DIALOGUES));
    s->paramValues.insert_or_assign("playerDialogue", PLAYER_DIALOGUE_STOPPED);
    s->paramValues.insert_or_assign("radioDialogue", RADIO_DIALOGUE_STOPPED);
    s->paramValues.insert_or_assign("line", 0);
    s->idEvent.emplace_back(CSHOTGUN_SHOOT);
    s->maskSounds.emplace_back(SOUND_DEFAULT);
    s->idEvent.emplace_back(CSHOTGUN_RELOAD);
    s->maskSounds.emplace_back(SOUND_NOT_RELEASE);
    s->idInstance.emplace_back(sFac->createInstanceFromEvent(CSHOTGUN_RELOAD, false, Transform{std::vector<float>{0,0,0,0,0,0,0,0,0}}));
    s->idEvent.emplace_back(REVOLVER_SHOOT);
    s->maskSounds.emplace_back(SOUND_DEFAULT);
    s->idEvent.emplace_back(REVOLVER_RELOAD);
    s->maskSounds.emplace_back(SOUND_NOT_RELEASE);
    s->idInstance.emplace_back(sFac->createInstanceFromEvent(REVOLVER_RELOAD, false, Transform{std::vector<float>{0,0,0,0,0,0,0,0,0}}));
    s->idEvent.emplace_back(APISTOL_SHOOT);
    s->maskSounds.emplace_back(SOUND_DEFAULT);
    s->idEvent.emplace_back(M16_SHOOT);
    s->maskSounds.emplace_back(SOUND_DEFAULT);
    s->idEvent.emplace_back(M16_RELOAD);
    s->maskSounds.emplace_back(SOUND_NOT_RELEASE);
    s->idInstance.emplace_back(sFac->createInstanceFromEvent(M16_RELOAD, false, Transform{std::vector<float>{0,0,0,0,0,0,0,0,0}}));
    s->idEvent.emplace_back(SHOTGUN_SHOOT);
    s->maskSounds.emplace_back(SOUND_DEFAULT);
    s->idEvent.emplace_back(SHOTGUN_RELOAD);
    s->maskSounds.emplace_back(SOUND_NOT_RELEASE);
    s->idInstance.emplace_back(sFac->createInstanceFromEvent(SHOTGUN_RELOAD, false, Transform{std::vector<float>{0,0,0,0,0,0,0,0,0}}));
    s->idEvent.emplace_back(BAZOOKA_SHOOT);
    s->maskSounds.emplace_back(SOUND_DEFAULT);
    s->idEvent.emplace_back(BAZOOKA_RELOAD);
    s->maskSounds.emplace_back(SOUND_NOT_RELEASE);
    s->idInstance.emplace_back(sFac->createInstanceFromEvent(BAZOOKA_RELOAD, false, Transform{std::vector<float>{0,0,0,0,0,0,0,0,0}}));
    s->idEvent.emplace_back(WEAPON_SWITCH);
    s->maskSounds.emplace_back(SOUND_DEFAULT);
    s->idEvent.emplace_back(LMACHINEGUN_SHOOT);
    s->maskSounds.emplace_back(SOUND_DEFAULT);
    s->idEvent.emplace_back(LMACHINEGUN_RELOAD);
    s->maskSounds.emplace_back(SOUND_NOT_RELEASE);
    s->idInstance.emplace_back(sFac->createInstanceFromEvent(LMACHINEGUN_RELOAD, false, Transform{std::vector<float>{0,0,0,0,0,0,0,0,0}}));
    s->idEvent.emplace_back(HMACHINEGUN_SHOOT);
    s->maskSounds.emplace_back(SOUND_DEFAULT);
    s->idEvent.emplace_back(HMACHINEGUN_RELOAD);
    s->maskSounds.emplace_back(SOUND_NOT_RELEASE);
    s->idInstance.emplace_back(sFac->createInstanceFromEvent(HMACHINEGUN_RELOAD, false, Transform{std::vector<float>{0,0,0,0,0,0,0,0,0}}));
    s->idEvent.emplace_back(M1_SHOOT);
    s->maskSounds.emplace_back(SOUND_DEFAULT);
    s->idEvent.emplace_back(M1_RELOAD);
    s->maskSounds.emplace_back(SOUND_NOT_RELEASE);
    s->idInstance.emplace_back(sFac->createInstanceFromEvent(M1_RELOAD, false, Transform{std::vector<float>{0,0,0,0,0,0,0,0,0}}));
    s->idEvent.emplace_back(BUY);
    s->maskSounds.emplace_back(SOUND_DEFAULT);
    s->idEvent.emplace_back(LOW_HEALTH);
    s->maskSounds.emplace_back(SOUND_NOT_RELEASE);
    s->idInstance.emplace_back(sFac->createInstanceFromEvent(LOW_HEALTH, false, Transform{std::vector<float>{0,0,0,0,0,0,0,0,0}}));
    s->idEvent.emplace_back(ACTION_MUSIC);
    s->maskSounds.emplace_back(SOUND_NOT_RELEASE);
    s->maskSounds[s->maskSounds.size()-1] |= SOUND_PLAY;
    s->idInstance.emplace_back(sFac->createInstanceFromEvent(ACTION_MUSIC, false, Transform{std::vector<float>{0,0,0,0,0,0,0,0,0}}));
    s->idEvent.emplace_back(PAUSE_STATE);
    s->maskSounds.emplace_back(SOUND_PLAY);
    s->idInstance.emplace_back(sFac->createInstanceFromEvent(PAUSE_STATE, false, Transform{std::vector<float>{0,0,0,0,0,0,0,0,0}}));
    s->idEvent.emplace_back(LASER_SHOOT);
    s->maskSounds.emplace_back(SOUND_DEFAULT);
    s->idEvent.emplace_back(LASER_RELOAD);
    s->maskSounds.emplace_back(SOUND_NOT_RELEASE);
    s->idInstance.emplace_back(sFac->createInstanceFromEvent(LASER_RELOAD, false, Transform{std::vector<float>{0,0,0,0,0,0,0,0,0}}));
    s->idEvent.emplace_back(GET_OBJECT);
    s->maskSounds.emplace_back(SOUND_DEFAULT);
    s->idEvent.emplace_back(NEW_ROUND);
    s->maskSounds.emplace_back(SOUND_DEFAULT);
    s->idEvent.emplace_back(AGONIC_BREATH);
    s->maskSounds.emplace_back(SOUND_DEFAULT);
    s->idEvent.emplace_back(WEAPON_NOAMMO);
    s->maskSounds.emplace_back(SOUND_DEFAULT);
    s->idEvent.emplace_back(BOOST_MACHINE);
    s->maskSounds.emplace_back(SOUND_DEFAULT);
    s->paramValues.insert_or_assign("Mejora", 0);
    
    // Camera component
    CameraComponent &camCmp = entity_man->getComponent<CameraComponent>(entity);
    camCmp.cameraNode.createCamera(GraphicEngine::Instance());
    camCmp.cameraNode.addYaw(transform.ry*PI/180);

    // Weapon component
    WeaponComponent &wcomp = entity_man->template getComponent<WeaponComponent>(entity);

    wcomp.firstWeapon   = std::unique_ptr<WeaponData>( new WeaponData() );
    *wcomp.firstWeapon  = WeaponTemplates::weapons[WTEMPLATE_ID_BASIC_GUN];
    wcomp.firstWeapon->weaponNode = createMesh(WeaponTemplates::WEAPON_MODEL[wcomp.firstWeapon->weaponID-1].c_str(), 
                                            WeaponTemplates::WEAPON_TEXTURE[wcomp.firstWeapon->weaponID-1].c_str());
    wcomp.firstWeapon->weaponNode->setCastShadows(false);

    wcomp.secondWeapon  = std::unique_ptr<WeaponData>( new WeaponData() );
    wcomp.activeWeapon  = wcomp.firstWeapon.get();


    createAnimation(entity, static_cast<int>(RunningAnimationWeapon::IDLE_ANIMATION), WeaponTemplates::WEAPON_IDLE_ANIM[wcomp.firstWeapon->weaponID-1].c_str(), true, true, true);
    createAnimation(entity, static_cast<int>(RunningAnimationWeapon::SHOOT_ANIMATION), WeaponTemplates::WEAPON_SHOT_ANIM[wcomp.firstWeapon->weaponID-1].c_str(), false, true, false);
    createAnimation(entity, static_cast<int>(RunningAnimationWeapon::RELOAD_ANIMATION), WeaponTemplates::WEAPON_RELOAD_ANIM[wcomp.firstWeapon->weaponID-1].c_str(), false, true, false);
    

    // Collision component
    CollisionComponent *colCmp = &entity_man->template getComponent<CollisionComponent>(entity);
    phy_engine->addCollisionObject(colCmp, entity, PLAYER_COLLISION);
    phy_engine->addCapsuleColliderToObject(&(colCmp->collision), 0, 3.2f, 0, 7.f, 8.f);

    return entity;
}

template <typename Comps, typename Tags>
Entity& EntityFactory::createEnemyNormal(float spawnx, float spawny, float spawnz, uint8_t target_enemy_msk)
{
    return createEnemyNormal_imp(Comps{}, Tags{}, spawnx, spawny, spawnz, target_enemy_msk);
}

template <typename... Comps, typename... Tags>
Entity& EntityFactory::createEnemyNormal_imp(MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>, float spawnx, float spawny, float spawnz, uint8_t target_enemy_msk)
{

    Entity &entity = entity_man->createEntity(BasicComponent{.x = spawnx, .y = spawny, .z = spawnz});

    ((void)entity_man->addComponent<Comps>(entity, Comps{}), ...);
    ((void)entity_man->addTag<Tags>(entity), ...);

    // Create Simple Enemy Behaviour Tree Nodes
    std::unique_ptr<BehaviourTree> simpleEnemyBT (new BehaviourTree());

    auto * nodeCheckYPos        = &simpleEnemyBT.get()->template createNode <BTCheckTargetYPosition>();
    auto * nodeCheckTargetVis   = &simpleEnemyBT.get()->template createNode <BTCheckTargetVisible>();

    auto * nodeCheckAttDist     = &simpleEnemyBT.get()->template createNode <BTCheckAttackDist>();
    auto * nodeBasicAtt         = &simpleEnemyBT.get()->template createNode <BTBasicAttack>();
    auto * nodeSeqBasicAtt      = &simpleEnemyBT.get()->template createNode <BTNodeSequence>(nodeCheckAttDist, nodeBasicAtt);

    auto * nodeActionSeek       = &simpleEnemyBT.get()->template createNode <BTActionFind>();

    auto * nodeSelEnemyActions  = &simpleEnemyBT.get()->template createNode <BTNodeSelector>(nodeSeqBasicAtt, nodeActionSeek);

    auto * nodeSeqVisActions    = &simpleEnemyBT.get()->template createNode <BTNodeSequence>(nodeCheckYPos, nodeCheckTargetVis, nodeSelEnemyActions);

    auto * nodeCalculateWp      = &simpleEnemyBT.get()->template createNode <BTCalculateWaypoint>();
    auto * nodeArriveWp         = &simpleEnemyBT.get()->template createNode <BTArriveToWaypoint>();
    auto * nodeSeqWp            = &simpleEnemyBT.get()->template createNode <BTNodeSequence>(nodeCalculateWp, nodeArriveWp);

    simpleEnemyBT.get()->template createNode <BTNodeSelector> (nodeSeqVisActions, nodeSeqWp);

    //Attack stats NORMAL ENEMY
    AttackStatsComponent *atckstats = &entity_man->template getComponent<AttackStatsComponent>(entity);
    atckstats->basic_dmg    = NORMAL_ENEMY_DAMAGE;
    atckstats->basic_cd     = ENEMY_BASIC_AT_COOLDOWN;

    HealthComponent *hCmp = &entity_man->template getComponent<HealthComponent>(entity);
    hCmp->health = NORMAL_ENEMY_HEALTH;
    hCmp->infection = NORMAL_ENEMY_INFECTION;

    RenderComponent &rcomp = entity_man->template getComponent<RenderComponent>(entity);

    rcomp.node = createMesh((uint16_t)MODEL_ENEMY_NORMAL, (uint16_t)TEXTURE_ENEMY_NORMAL);
    rcomp.node.setScale(5, 5, 5);

    createAnimation(entity, static_cast<int>(RunningAnimationEnemy::WALK_ANIMATION), WALK_ANIMATIONS[ANIMATION_ENEMY_NORMAL_WALK], true, false, true);
    createAnimation(entity, static_cast<int>(RunningAnimationEnemy::ATTACK_ANIMATION), ATTACK_ANIMATIONS[ANIMATION_ENEMY_NORMAL_ATTACK], false, false, false);
    createAnimation(entity, static_cast<int>(RunningAnimationEnemy::ATTACK_ANIMATION1), ATTACK_ANIMATIONS[ANIMATION_ENEMY_NORMAL_ATTACK], false, false, false);
    createAnimation(entity, static_cast<int>(RunningAnimationEnemy::DEATH_ANIMATION), DEATH_ANIMATIONS[ANIMATION_ENEMY_NORMAL_WALK], false, false, false);

    int randomVel = std::rand()%6;
    MovementComponent &mcomp = entity_man->template getComponent<MovementComponent>(entity);
    mcomp.accel         = ENEMY_REGULAR_ACCEL;
    mcomp.maxVelocity    = randomVel + NORMAL_ENEMY_MAXVEL;
    mcomp.maxRunVelocity = randomVel + NORMAL_ENEMY_MAXRUNVEL;

    AIComponent &aicomp = entity_man->template getComponent<AIComponent>(entity);
    aicomp.behaviourTree = std::move(simpleEnemyBT);
    aicomp.attackCD = ENEMY_BASIC_AT_COOLDOWN;
    aicomp.targetMask = target_enemy_msk;

    CollisionComponent *colCmp = &entity_man->template getComponent<CollisionComponent>(entity);
    phy_engine->addCollisionObject(colCmp, entity, ENEMY_COLLISION);
    phy_engine->addBoxColliderToObject(&(colCmp->collision), 0, 6.4, 0, 3.5f, 6.4f, 1.2f);

    //Head collision
    phy_engine->addBoxColliderToObject(&(colCmp->collision), 0, 14.6, 0.6, 1.5f, 1.8f, 1.7f, SET_COLLIDER_HEAD);

    SoundComponent *soundCmp = &entity_man->template getComponent<SoundComponent>(entity);
    SoundFactory* sFac = SoundFactory::Instance();

    BasicComponent *basicCmp = &entity_man->template getComponent<BasicComponent>(entity);

    soundCmp->idEvent.emplace_back(ZOMBIE_PAIN);
    soundCmp->maskSounds.emplace_back(SOUND_3D);
    soundCmp->idEvent.emplace_back(ZOMBIE_ATTACK);
    soundCmp->maskSounds.emplace_back(SOUND_3D);
    soundCmp->idEvent.emplace_back(ZOMBIE_STEPS);
    soundCmp->maskSounds.emplace_back(SOUND_NOT_RELEASE);
    soundCmp->idInstance.emplace_back(sFac->createInstanceFromEvent(ZOMBIE_STEPS, true, Transform{std::vector<float>{basicCmp->x, basicCmp->y, basicCmp->z, (float)basicCmp->orientation, 0, (float)basicCmp->verticalOrientation, 0, 0, 0}}));
    soundCmp->paramValues.insert_or_assign("velocidad", 0);
    soundCmp->idEvent.emplace_back(ZOMBIE_SCREAM_ATTACK);
    soundCmp->maskSounds.emplace_back(SOUND_3D);
    return entity;
}

template <typename Comps, typename Tags>
Entity& EntityFactory::createEnemyExplosive(float spawnx, float spawny, float spawnz)
{
    return createEnemyExplosive_imp(Comps{}, Tags{}, spawnx, spawny, spawnz);
}

template <typename... Comps, typename... Tags>
Entity& EntityFactory::createEnemyExplosive_imp(MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>, float spawnx, float spawny, float spawnz)
{

    Entity &entity = entity_man->createEntity(BasicComponent{.x = spawnx, .y = spawny, .z = spawnz});

    ((void)entity_man->addComponent<Comps>(entity, Comps{}), ...);
    ((void)entity_man->addTag<Tags>(entity), ...);

    // Create Simple Enemy Behaviour Tree Nodes
    std::unique_ptr<BehaviourTree> simpleEnemyBT (new BehaviourTree());

    auto * nodeCheckYPos        = &simpleEnemyBT.get()->template createNode <BTCheckTargetYPosition>();
    auto * nodeCheckTargetVis   = &simpleEnemyBT.get()->template createNode <BTCheckTargetVisible>();

    auto * nodeCheckExplAtt     = &simpleEnemyBT.get()->template createNode <BTCheckExplosiveAttack>();
    auto * nodeExplAtt          = &simpleEnemyBT.get()->template createNode <BTExplosiveAttack>();
    auto * nodeSeqExplosiveAtt  = &simpleEnemyBT.get()->template createNode <BTNodeSequence>(nodeCheckExplAtt, nodeExplAtt);

    auto * nodeActionSeek       = &simpleEnemyBT.get()->template createNode <BTActionFind>();

    auto * nodeSelEnemyActions  = &simpleEnemyBT.get()->template createNode <BTNodeSelector>(nodeSeqExplosiveAtt, nodeActionSeek);

    auto * nodeSeqVisActions    = &simpleEnemyBT.get()->template createNode <BTNodeSequence>(nodeCheckYPos, nodeCheckTargetVis, nodeSelEnemyActions);

    auto * nodeCalculateWp      = &simpleEnemyBT.get()->template createNode <BTCalculateWaypoint>();
    auto * nodeArriveWp         = &simpleEnemyBT.get()->template createNode <BTArriveToWaypoint>();
    auto * nodeSeqWp            = &simpleEnemyBT.get()->template createNode <BTNodeSequence>(nodeCalculateWp, nodeArriveWp);

    simpleEnemyBT->template createNode <BTNodeSelector> (nodeSeqVisActions, nodeSeqWp);

    //Attack stats EXPLOSIVE ENEMY
    AttackStatsComponent *atckstats = &entity_man->template getComponent<AttackStatsComponent>(entity);
    atckstats->special_dmg  = EXPLOSIVE_ENEMY_DAMAGE;
    atckstats->special_cd   = ENEMY_EX_AT_DURATION;

    HealthComponent *hCmp = &entity_man->template getComponent<HealthComponent>(entity);
    hCmp->health = EXPLOSIVE_ENEMY_HEALTH;
    hCmp->infection = EXPLOSIVE_ENEMY_INFECTION;

    RenderComponent &rcomp = entity_man->template getComponent<RenderComponent>(entity);
    rcomp.node = createMesh(MODEL_ENEMY_EXPLOSIVE, TEXTURE_ENEMY_EXPLOSIVE);
    rcomp.node.setScale(5, 5, 5);
    rcomp.emitter.createParticleEmitter(ParticleEmitterType::ExplosionParticle);
    rcomp.emitter.setActive(false);

    createAnimation(entity, static_cast<int>(RunningAnimationEnemy::WALK_ANIMATION), WALK_ANIMATIONS[ANIMATION_ENEMY_EXPLOSIVE_WALK], true, false, true);
    createAnimation(entity, static_cast<int>(RunningAnimationEnemy::ATTACK_ANIMATION), ATTACK_ANIMATIONS[ANIMATION_ENEMY_EXPLOSIVE_ATTACK], false, false, false);
    createAnimation(entity, static_cast<int>(RunningAnimationEnemy::ATTACK_ANIMATION1), ATTACK_ANIMATIONS[ANIMATION_ENEMY_EXPLOSIVE_ATTACK], false, false, false);
    createAnimation(entity, static_cast<int>(RunningAnimationEnemy::DEATH_ANIMATION), DEATH_ANIMATIONS[ANIMATION_ENEMY_EXPLOSIVE_WALK], false, false, false);
    MovementComponent &mcomp = entity_man->template getComponent<MovementComponent>(entity);
    mcomp.accel         = ENEMY_REGULAR_ACCEL;
    mcomp.maxVelocity   = EXPLOSIVE_ENEMY_MAXVEL;
    mcomp.maxRunVelocity= EXPLOSIVE_ENEMY_MAXRUNVEL;

    NavigationComponent &navcomp = entity_man->template getComponent<NavigationComponent>(entity);
    navcomp.pathWeights[0] = MAX_WEIGHT_VALUE;

    AIComponent &aicomp = entity_man->template getComponent<AIComponent>(entity);
    aicomp.behaviourTree = std::move(simpleEnemyBT);
    aicomp.arrivalRadius = 7;

    CollisionComponent *colCmp = &entity_man->template getComponent<CollisionComponent>(entity);
    phy_engine->addCollisionObject(colCmp, entity, ENEMY_COLLISION);
    phy_engine->addBoxColliderToObject(&(colCmp->collision), 0, 7.7, 0, 3.f, 7.7f, 2.6f);

    //Head collision
    phy_engine->addBoxColliderToObject(&(colCmp->collision), 0, 17.4, 0.6, 1.6f, 2.0f, 2.0f, SET_COLLIDER_HEAD);

    SoundComponent *soundCmp = &entity_man->template getComponent<SoundComponent>(entity);
    SoundFactory* sFac = SoundFactory::Instance();
    BasicComponent *basicCmp = &entity_man->template getComponent<BasicComponent>(entity);

    soundCmp->idEvent.emplace_back(ZOMBIE_PAIN);
    soundCmp->maskSounds.emplace_back(SOUND_3D);
    soundCmp->idEvent.emplace_back(ZOMBIE_ATTACK);
    soundCmp->maskSounds.emplace_back(SOUND_3D);
    soundCmp->idEvent.emplace_back(E_ZOMBIE_STEPS);
    soundCmp->maskSounds.emplace_back(SOUND_NOT_RELEASE);
    soundCmp->idInstance.emplace_back(sFac->createInstanceFromEvent(E_ZOMBIE_STEPS, true, Transform{std::vector<float>{basicCmp->x, basicCmp->y, basicCmp->z, (float)basicCmp->orientation, 0, (float)basicCmp->verticalOrientation, 0, 0, 0}}));
    soundCmp->paramValues.insert_or_assign("velocidad", 0);

    return entity;
}

template <typename Comps, typename Tags>
Entity& EntityFactory::createEnemyRugby(float spawnx, float spawny, float spawnz)
{
    return createEnemyRugby_imp(Comps{}, Tags{}, spawnx, spawny, spawnz);
}

template <typename... Comps, typename... Tags>
Entity& EntityFactory::createEnemyRugby_imp(MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>, float spawnx, float spawny, float spawnz)
{

    Entity &entity = entity_man->createEntity(BasicComponent{.x = spawnx, .y = spawny, .z = spawnz});

    ((void)entity_man->addComponent<Comps>(entity, Comps{}), ...);
    ((void)entity_man->addTag<Tags>(entity), ...);

    // Create Simple Enemy Behaviour Tree Nodes
    std::unique_ptr<BehaviourTree> simpleEnemyBT (new BehaviourTree());

    auto * nodeCheckYPos        = &simpleEnemyBT.get()->template createNode <BTCheckTargetYPosition>();
    auto * nodeCheckTargetVis   = &simpleEnemyBT.get()->template createNode <BTCheckTargetVisible>();

    auto * nodeCheckAttDist     = &simpleEnemyBT.get()->template createNode <BTCheckAttackDist>();
    auto * nodeBasicAtt         = &simpleEnemyBT.get()->template createNode <BTBasicAttack>();
    auto * nodeSeqRugbyAtt      = &simpleEnemyBT.get()->template createNode <BTNodeSequence>(nodeCheckAttDist, nodeBasicAtt);

    auto * nodeCheckAttSlow     = &simpleEnemyBT.get()->template createNode <BTCheckSlowAttackCD>();
    auto * nodeSpecialAtt       = &simpleEnemyBT.get()->template createNode <BTSpecialAttack>();
    auto * nodeSeqSlowAtt       = &simpleEnemyBT.get()->template createNode <BTNodeSequence>(nodeCheckAttSlow, nodeSpecialAtt);

    auto * nodeActionSeek       = &simpleEnemyBT->template createNode <BTActionFind>();

    auto * nodeSelEnemyActions  = &simpleEnemyBT->template createNode <BTNodeSelector>(nodeSeqRugbyAtt, nodeSeqSlowAtt, nodeActionSeek);

    auto * nodeSeqVisActions    = &simpleEnemyBT->template createNode <BTNodeSequence>(nodeCheckYPos, nodeCheckTargetVis, nodeSelEnemyActions);

    auto * nodeCalculateWp      = &simpleEnemyBT->template createNode <BTCalculateWaypoint>();
    auto * nodeArriveWp         = &simpleEnemyBT->template createNode <BTArriveToWaypoint>();
    auto * nodeSeqWp            = &simpleEnemyBT->template createNode <BTNodeSequence>(nodeCalculateWp, nodeArriveWp);

    simpleEnemyBT->template createNode <BTNodeSelector> (nodeSeqVisActions, nodeSeqWp);

    //Attack stats RUGBY ENEMY
    AttackStatsComponent *atckstats = &entity_man->template getComponent<AttackStatsComponent>(entity);
    atckstats->basic_dmg    = RUGBY_ENEMY_DAMAGE;
    atckstats->basic_cd     = ENEMY_RUGBY_AT_COOLDOWN;
    atckstats->special_dmg  = RUGBY_ENEMY_DAMAGE_SLOW;
    atckstats->special_cd   = ENEMY_RUGBY_AT_SLOW_COOLDOWN;

    HealthComponent *hCmp = &entity_man->template getComponent<HealthComponent>(entity);
    hCmp->health = RUGBY_ENEMY_HEALTH;
    hCmp->infection = RUGBY_ENEMY_INFECTION;

    RenderComponent &rcomp = entity_man->template getComponent<RenderComponent>(entity);
    rcomp.node = createMesh(MODEL_ENEMY_RUGBY, TEXTURE_ENEMY_RUGBY);
    rcomp.node.setScale(3.8, 3.8, 3.8);

    createAnimation(entity, static_cast<int>(RunningAnimationEnemy::WALK_ANIMATION), WALK_ANIMATIONS[ANIMATION_ENEMY_TANK_WALK], true, false, true);
    createAnimation(entity, static_cast<int>(RunningAnimationEnemy::ATTACK_ANIMATION), ATTACK_ANIMATIONS[ANIMATION_ENEMY_TANK_ATTACK], false, false, false);
    createAnimation(entity, static_cast<int>(RunningAnimationEnemy::ATTACK_ANIMATION1), ATTACK_ANIMATIONS[ANIMATION_ENEMY_TANK_SLOW_ATTACK], false, false, false);
    createAnimation(entity, static_cast<int>(RunningAnimationEnemy::DEATH_ANIMATION), DEATH_ANIMATIONS[ANIMATION_ENEMY_TANK_WALK], false, false, false);

    MovementComponent &mcomp = entity_man->template getComponent<MovementComponent>(entity);
    mcomp.accel         = ENEMY_REGULAR_ACCEL;
    mcomp.maxVelocity   = RUGBY_ENEMY_MAXVEL;
    mcomp.maxRunVelocity= RUGBY_ENEMY_MAXRUNVEL;

    AIComponent &aicomp = entity_man->template getComponent<AIComponent>(entity);
    aicomp.behaviourTree = std::move(simpleEnemyBT);
    aicomp.attackCD = ENEMY_RUGBY_AT_SLOW_COOLDOWN;

    CollisionComponent *colCmp = &entity_man->template getComponent<CollisionComponent>(entity);
    phy_engine->addCollisionObject(colCmp, entity, ENEMY_COLLISION);
    phy_engine->addBoxColliderToObject(&(colCmp->collision), 0, 11.f, 0, 5.1f, 11.f, 3.2f);

    //Head collision
    phy_engine->addBoxColliderToObject(&(colCmp->collision), 0, 24.5f, 0, 2.5f, 2.5f, 2.5f, SET_COLLIDER_HEAD);

    SoundComponent *soundCmp = &entity_man->template getComponent<SoundComponent>(entity);
    SoundFactory* sFac = SoundFactory::Instance();

    BasicComponent *basicCmp = &entity_man->template getComponent<BasicComponent>(entity);

    soundCmp->idEvent.emplace_back(ZOMBIE_PAIN);
    soundCmp->maskSounds.emplace_back(SOUND_3D);
    soundCmp->idEvent.emplace_back(T_ZOMBIE_ATTACK);
    soundCmp->maskSounds.emplace_back(SOUND_3D);
    soundCmp->idEvent.emplace_back(T_ZOMBIE_STEPS);
    soundCmp->maskSounds.emplace_back(SOUND_NOT_RELEASE);
    soundCmp->idInstance.emplace_back(sFac->createInstanceFromEvent(T_ZOMBIE_STEPS, true, Transform{std::vector<float>{basicCmp->x, basicCmp->y, basicCmp->z, (float)basicCmp->orientation, 0, (float)basicCmp->verticalOrientation, 0, 0, 0}}));
    soundCmp->paramValues.insert_or_assign("velocidad", 0);
    soundCmp->idEvent.emplace_back(ZOMBIE_SCREAM_ATTACK);
    soundCmp->maskSounds.emplace_back(SOUND_3D);
    soundCmp->idEvent.emplace_back(T_ZOMBIE_SPECIAL);
    soundCmp->maskSounds.emplace_back(SOUND_3D);

    return entity;
}

template <typename Comps, typename Tags>
Entity& EntityFactory::createEnemyThrower(float spawnx, float spawny, float spawnz)
{
    return createEnemyThrower_imp(Comps{}, Tags{}, spawnx, spawny, spawnz);
}

template <typename... Comps, typename... Tags>
Entity& EntityFactory::createEnemyThrower_imp(MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>, float spawnx, float spawny, float spawnz)
{

    Entity &entity = entity_man->createEntity(BasicComponent{.x = spawnx, .y = spawny, .z = spawnz});

    ((void)entity_man->addComponent<Comps>(entity, Comps{}), ...);
    ((void)entity_man->addTag<Tags>(entity), ...);

    // Create Simple Enemy Behaviour Tree Nodes
    std::unique_ptr<BehaviourTree> simpleEnemyBT (new BehaviourTree());

    auto * nodeCheckYPos        = &simpleEnemyBT.get()->template createNode <BTCheckTargetYPosition>();
    auto * nodeCheckTargetVis   = &simpleEnemyBT.get()->template createNode <BTCheckTargetVisible>();

    auto * nodeCheckAttDist     = &simpleEnemyBT.get()->template createNode <BTCheckAttackDist>();
    auto * nodeBasicAtt         = &simpleEnemyBT.get()->template createNode <BTBasicAttack>();
    auto * nodeSeqBasicAtt      = &simpleEnemyBT.get()->template createNode <BTNodeSequence>(nodeCheckAttDist, nodeBasicAtt);

    auto * nodeCheckAttSpit     = &simpleEnemyBT.get()->template createNode <BTCheckSpitAttack>();
    auto * nodeSpecialAtt       = &simpleEnemyBT.get()->template createNode <BTSpecialAttack>();
    auto * nodeSeqSpitAtt       = &simpleEnemyBT.get()->template createNode <BTNodeSequence>(nodeCheckAttSpit, nodeSpecialAtt);

    auto * nodeActionSeek       = &simpleEnemyBT.get()->template createNode <BTActionFind>();

    auto * nodeSelEnemyActions  = &simpleEnemyBT.get()->template createNode <BTNodeSelector>(nodeSeqBasicAtt, nodeSeqSpitAtt, nodeActionSeek);

    auto * nodeSeqVisActions    = &simpleEnemyBT.get()->template createNode <BTNodeSequence>(nodeCheckYPos, nodeCheckTargetVis, nodeSelEnemyActions);

    auto * nodeCalculateWp      = &simpleEnemyBT.get()->template createNode <BTCalculateWaypoint>();
    auto * nodeArriveWp         = &simpleEnemyBT.get()->template createNode <BTArriveToWaypoint>();
    auto * nodeSeqWp            = &simpleEnemyBT.get()->template createNode <BTNodeSequence>(nodeCalculateWp, nodeArriveWp);

    simpleEnemyBT->template createNode <BTNodeSelector> (nodeSeqVisActions, nodeSeqWp);

    //Attack stats NORMAL ENEMY
    AttackStatsComponent *atckstats = &entity_man->template getComponent<AttackStatsComponent>(entity);
    atckstats->basic_dmg    = THROWER_ENEMY_DAMAGE;
    atckstats->basic_cd     = ENEMY_THROWER_AT_COOLDOWN;
    atckstats->special_dmg  = THROWER_ENEMY_DAMAGE_SPIT;
    atckstats->special_cd   = ENEMY_THROWER_AT_SPIT_COOLDOWN;

    HealthComponent *hCmp = &entity_man->template getComponent<HealthComponent>(entity);
    hCmp->health = THROWER_ENEMY_HEALTH;
    hCmp->infection = THROWER_ENEMY_INFECTION;

    RenderComponent &rcomp = entity_man->template getComponent<RenderComponent>(entity);
    rcomp.node = createMesh(MODEL_ENEMY_THROWER, TEXTURE_ENEMY_THROWER);
    rcomp.node.setScale(4.7, 5.2, 4.2);

    createAnimation(entity, static_cast<int>(RunningAnimationEnemy::WALK_ANIMATION), WALK_ANIMATIONS[ANIMATION_ENEMY_THROWER_WALK], true, false, true);
    createAnimation(entity, static_cast<int>(RunningAnimationEnemy::ATTACK_ANIMATION), ATTACK_ANIMATIONS[ANIMATION_ENEMY_THROWER_ATTACK], false, false, false);
    createAnimation(entity, static_cast<int>(RunningAnimationEnemy::ATTACK_ANIMATION1), ATTACK_ANIMATIONS[ANIMATION_ENEMY_THROWER_SPIT_ATTACK], false, false, false);
    createAnimation(entity, static_cast<int>(RunningAnimationEnemy::DEATH_ANIMATION), DEATH_ANIMATIONS[ANIMATION_ENEMY_THROWER_WALK], false, false, false);

    MovementComponent &mcomp = entity_man->template getComponent<MovementComponent>(entity);
    mcomp.accel         = ENEMY_REGULAR_ACCEL;
    mcomp.maxVelocity   = THROWER_ENEMY_MAXVEL;
    mcomp.maxRunVelocity= THROWER_ENEMY_MAXRUNVEL;

    NavigationComponent &navcomp = entity_man->template getComponent<NavigationComponent>(entity);
    navcomp.pathWeights[0] = -MAX_WEIGHT_VALUE;

    AIComponent &aicomp = entity_man->template getComponent<AIComponent>(entity);
    aicomp.behaviourTree = std::move(simpleEnemyBT);
    aicomp.attackCD = ENEMY_THROWER_AT_SPIT_COOLDOWN;

    CollisionComponent *colCmp = &entity_man->template getComponent<CollisionComponent>(entity);
    phy_engine->addCollisionObject(colCmp, entity, ENEMY_COLLISION);
    phy_engine->addBoxColliderToObject(&(colCmp->collision), 0, 8.3, 0, 2.7f, 8.3f, 1.2f);

    //Head collision
    phy_engine->addBoxColliderToObject(&(colCmp->collision), 0, 19.4, 0.8, 1.6f, 2.8f, 1.6f, SET_COLLIDER_HEAD);
    

    SoundComponent *soundCmp = &entity_man->template getComponent<SoundComponent>(entity);
    SoundFactory* sFac = SoundFactory::Instance();

    BasicComponent *basicCmp = &entity_man->template getComponent<BasicComponent>(entity);

    soundCmp->idEvent.emplace_back(ZOMBIE_PAIN);
    soundCmp->maskSounds.emplace_back(SOUND_3D);
    soundCmp->idEvent.emplace_back(ZOMBIE_ATTACK);
    soundCmp->maskSounds.emplace_back(SOUND_3D);
    soundCmp->idEvent.emplace_back(ZOMBIE_STEPS);
    soundCmp->maskSounds.emplace_back(SOUND_NOT_RELEASE);
    soundCmp->idInstance.emplace_back(sFac->createInstanceFromEvent(ZOMBIE_STEPS, true, Transform{std::vector<float>{basicCmp->x, basicCmp->y, basicCmp->z, (float)basicCmp->orientation, 0, (float)basicCmp->verticalOrientation, 0, 0, 0}}));
    soundCmp->paramValues.insert_or_assign("velocidad", 0);
    soundCmp->idEvent.emplace_back(ZOMBIE_SCREAM_ATTACK);
    soundCmp->maskSounds.emplace_back(SOUND_3D);
    soundCmp->idEvent.emplace_back(ZOMBIE_SPIT);
    soundCmp->maskSounds.emplace_back(SOUND_3D);
    return entity;
}

template <typename Comps, typename Tags>
Entity* EntityFactory::createEnemyAttack(BasicComponent* bc, std::vector<float> atparams, uint16_t attackMask)
{
    return createEnemyAttack_imp(Comps{}, Tags{}, bc, atparams, attackMask);
}

template <typename... Comps, typename... Tags>
Entity* EntityFactory::createEnemyAttack_imp(MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>, BasicComponent* bc, std::vector<float> atparams, uint16_t attackMask)
{

    Entity &entity = entity_man->createEntity(BasicComponent{.x = bc->x, .y = bc->y, .z = bc->z,
                .orientation = bc->orientation, .verticalOrientation = bc->verticalOrientation});

    ((void)entity_man->addComponent<Comps>(entity, Comps{}), ...);
    ((void)entity_man->addTag<Tags>(entity), ...);

    AttackComponent *atCmp = &entity_man->template getComponent<AttackComponent>(entity);
    atCmp->maxDurationAttack = atparams[8];
    atCmp->damage       = atparams[6];
    atCmp->infection    = atparams[7];
    atCmp->damageTo     = attackMask;

    CollisionComponent *colCmp = &entity_man->template getComponent<CollisionComponent>(entity);
    phy_engine->addCollisionObject(colCmp, entity, ATTACK_COLLISION);
    phy_engine->addBoxColliderToObject(&(colCmp->collision), atparams[0], atparams[1], atparams[2], atparams[3], atparams[4], atparams[5]);


    return &entity;
}

template <typename Comps, typename Tags>
Entity* EntityFactory::createProjectileAttack(BasicComponent* bc, std::vector<float> atparams, uint16_t attackMask, uint16_t collisionMask)
{
    return createProjectileAttack_imp(Comps{}, Tags{}, bc, atparams, attackMask, collisionMask);
}

template <typename... Comps, typename... Tags>
Entity* EntityFactory::createProjectileAttack_imp(MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>, BasicComponent* bc, std::vector<float> atparams, uint16_t attackMask, uint16_t collisionMask)
{

    Entity &entity = entity_man->createEntity(BasicComponent{.x = bc->x + atparams[0], .y = bc->y + atparams[1], .z = bc->z + atparams[2],
                .orientation = bc->orientation, .verticalOrientation = bc->verticalOrientation});

    ((void)entity_man->addComponent<Comps>(entity, Comps{}), ...);
    ((void)entity_man->addTag<Tags>(entity), ...);

    AttackComponent *atCmp = &entity_man->template getComponent<AttackComponent>(entity);
    atCmp->maxDurationAttack = atparams[8];
    atCmp->damage       = atparams[6];
    atCmp->infection    = atparams[7];
    atCmp->damageTo     = attackMask;

    CollisionComponent *colCmp = &entity_man->template getComponent<CollisionComponent>(entity);
    phy_engine->addCollisionObject(colCmp, entity, collisionMask);
    phy_engine->addBoxColliderToObject(&(colCmp->collision), 0, 0, 0, atparams[3], atparams[4], atparams[5]);

    MovementComponent *movCmp = &entity_man->template getComponent<MovementComponent>(entity);
    movCmp->moving = true;
    RenderComponent &rcomp = entity_man->template getComponent<RenderComponent>(entity);
    SoundComponent &scomp = entity_man->template getComponent<SoundComponent>(entity);
    if(collisionMask==PROJECTILE_SLOW_COLLISION){
        rcomp.node = createMesh(MODEL_SLOW_ATTACK, TEXTURE_SLOW_ATTACK);
        rcomp.node.setScale(6, 6, 6);
        movCmp->maxVelocity = RUGBY_PROJECTILE_SPEED;
    }
    else if(collisionMask==PROJECTILE_SPIT_COLLISION){
        rcomp.node = createMesh(MODEL_SPIT_ATTACK, TEXTURE_SPIT_ATTACK);
        rcomp.node.setScale(3, 3, 3);
        movCmp->maxVelocity = THROWER_PROJECTILE_SPEED;
        movCmp->gravity = 0;
    }
    else if(collisionMask==PROJECTILE_ROCKET_COLLISION) {
        entity.addTag<BazookaProjectileTag>();
        rcomp.node = createMesh(MODEL_ROCKET_ATTACK, TEXTURE_ROCKET_ATTACK);
        movCmp->maxVelocity = ROCKET_PROJECTILE_SPEED;
        movCmp->gravity = 0;
        scomp.idEvent.emplace_back(BAZOOKA_EXPLOSION);
        scomp.maskSounds.emplace_back(SOUND_3D);
    }

    return &entity;
}

template <typename Comps, typename Tags>
Entity* EntityFactory::createExplosion(size_t source_id, BasicComponent* bc, std::vector<float> atparams, uint16_t attackMask)
{
    return createExplosion_imp(Comps{}, Tags{}, source_id, bc, atparams, attackMask);
}

template <typename... Comps, typename... Tags>
Entity* EntityFactory::createExplosion_imp(MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>, size_t source_id, BasicComponent* bc, std::vector<float> atparams, uint16_t attackMask)
{
    Entity* source = entity_man->getEntityById(source_id);
    //Check entity explosion source
    if(source->template hasTag<ExplosiveBarrelTag>() || source->template hasTag<BazookaProjectileTag>()){
        RenderComponent* rcomp = &entity_man->template getComponent<RenderComponent>(*source);
        rcomp->emitter.createParticleEmitter(ParticleEmitterType::ExplosionParticle);
        rcomp->emitter.setActiveForTime(0.2);
        if(source->template hasTag<BazookaProjectileTag>()) rcomp->node.setVisible(false);
        SoundComponent* s = &entity_man->template getComponent<SoundComponent>(*source);
        s->maskSounds[M_EXPLOSION] |= SOUND_PLAY;
    }

    Entity &entity = entity_man->createEntity(BasicComponent{.x = bc->x, .y = bc->y, .z = bc->z,
                .orientation = bc->orientation, .verticalOrientation = bc->verticalOrientation});

    ((void)entity_man->addComponent<Comps>(entity, Comps{}), ...);
    ((void)entity_man->addTag<Tags>(entity), ...);

    AttackComponent *atCmp = &entity_man->template getComponent<AttackComponent>(entity);
    atCmp->maxDurationAttack = ENEMY_BASIC_AT_DURATION;
    atCmp->damage       = atparams[4];
    atCmp->infection    = atparams[5];
    atCmp->damageTo     = attackMask;

    CollisionComponent *colCmp = &entity_man->template getComponent<CollisionComponent>(entity);
    phy_engine->addCollisionObject(colCmp, entity, ATTACK_COLLISION);
    phy_engine->addSphereColliderToObject(&(colCmp->collision), atparams[0], atparams[1], atparams[2], atparams[3]);


    return &entity;
}

template <typename Comps, typename Tags>
Entity* EntityFactory::createBarrel(uint8_t barrel_type, const Transform& barrel_trasform)
{
    return createBarrel_imp(barrel_type,barrel_trasform, Comps{}, Tags{});
}

template <typename... Comps, typename... Tags>
Entity* EntityFactory::createBarrel_imp(uint8_t barrel_type, const Transform& barrel_trasform, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>)
{
    Entity &entity = entity_man->createEntity(BasicComponent{   .x = barrel_trasform.x, .prevx = barrel_trasform.x, 
                                                                .y = barrel_trasform.y, .prevy = barrel_trasform.y, 
                                                                .z = barrel_trasform.z, .prevz = barrel_trasform.z,
                                                                .orientation = barrel_trasform.ry*PI/180, .prevorientation = barrel_trasform.ry*PI/180});

    ((void)entity_man->addComponent<Comps>(entity, Comps{}), ...);

    HealthComponent &hpcomp = entity_man->template getComponent<HealthComponent>(entity);
    hpcomp.health = BARREL_HP;

    AIComponent &aicomp = entity_man->template getComponent<AIComponent>(entity);
    // Create Simple Enemy Behaviour Tree Nodes
    std::unique_ptr<BehaviourTree> simpleEnemyBT (new BehaviourTree());
    int texture = TEXTURE_BARREL_EXP;
    int model   = MODEL_BARREL_EXP;
    if(barrel_type == EXPLOSIVE_BARREL_ID){
        entity_man->addTag<ExplosiveBarrelTag>(entity);
        SoundComponent *s = &entity_man->getComponent<SoundComponent>(entity);
        s->idEvent.emplace_back(EXPLOSION);
        s->maskSounds.emplace_back(SOUND_3D);

        //Create nodes for explosive barrel
        auto * nodeCheckCurrentHP   = &simpleEnemyBT.get()->template createNode <BTCheckCurrentHealth>();
        auto * nodeCreateExplosion  = &simpleEnemyBT.get()->template createNode <BTCreateExplosion>();
        auto * nodeExplosiveBarrCD  = &simpleEnemyBT.get()->template createNode <BTExplosiveBarrelCD>();
        simpleEnemyBT->template createNode <BTNodeSequence> (nodeCheckCurrentHP, nodeCreateExplosion, nodeExplosiveBarrCD);

        aicomp.attackCD = EXPLOSIVE_BARREL_DURATION;

       
    }else if(barrel_type == TARGET_BARREL_ID){
        texture = TEXTURE_BARREL_TAR;
        model   = MODEL_BARREL_SENY;
        entity_man->addTag<TargetBarrelTag>(entity);

        //Create nodes for target barrel
        auto * nodeCheckCurrentHP   = &simpleEnemyBT.get()->template createNode <BTCheckCurrentHealth>();
        auto * nodeTargetBarrCD     = &simpleEnemyBT.get()->template createNode <BTTargetBarrelCD>();
        simpleEnemyBT->template createNode <BTNodeSequence> (nodeCheckCurrentHP, nodeTargetBarrCD);

        SoundComponent *s = &entity_man->getComponent<SoundComponent>(entity);
        s->idEvent.emplace_back(TARGET_BARREL);
        s->maskSounds.emplace_back(SOUND_3D);

        aicomp.attackCD = TARGET_BARREL_DURATION + TARGET_BARREL_CD;
    }

    aicomp.behaviourTree = std::move(simpleEnemyBT);

    RenderComponent &rcomp2 = entity_man->template getComponent<RenderComponent>(entity);
    rcomp2.node = createMesh(model, texture);
    rcomp2.node.setScale(3, 3, 3);

    //Create animations
    if(model == MODEL_BARREL_EXP){
        createAnimation(entity, static_cast<int>(RunningAnimationBarrel::IDLE_ANIMATION), MAP_PROPS_MEDIA_PATH("models/barril_explosivo_estatico.fbx"), true, false, true);
        createAnimation(entity, static_cast<int>(RunningAnimationBarrel::BREAK_ANIMATION), MAP_PROPS_MEDIA_PATH("models/barril_explosivo.fbx"), false, false, false);
        createAnimation(entity, static_cast<int>(RunningAnimationBarrel::RESTORE_ANIMATION), MAP_PROPS_MEDIA_PATH("models/barril_explosivo_reset.fbx"), false, false, false);
        createAnimation(entity, static_cast<int>(RunningAnimationBarrel::IDLE_ANIMATION_BREAK), MAP_PROPS_MEDIA_PATH("models/barril_explosivo_estatico_roto.fbx"), true, false, false);
    }else{
        createAnimation(entity, static_cast<int>(RunningAnimationBarrel::IDLE_ANIMATION), MAP_PROPS_MEDIA_PATH("models/barril_senyuelo_estatico.fbx"), true, false, true);
        createAnimation(entity, static_cast<int>(RunningAnimationBarrel::BREAK_ANIMATION), MAP_PROPS_MEDIA_PATH("models/barril_senyuelo.fbx"), false, false, false);
        createAnimation(entity, static_cast<int>(RunningAnimationBarrel::RESTORE_ANIMATION), MAP_PROPS_MEDIA_PATH("models/barril_senyuelo_reset.fbx"), false, false, false);
        createAnimation(entity, static_cast<int>(RunningAnimationBarrel::IDLE_ANIMATION_BREAK), MAP_PROPS_MEDIA_PATH("models/barril_senyuelo_estatico_roto.fbx"), true, false, false);
    }
    
    CollisionComponent *colCmp = &entity_man->template getComponent<CollisionComponent>(entity);
    phy_engine->addCollisionObject(colCmp, entity, BARREL_COLLISION);
    phy_engine->addBoxColliderToObject(&(colCmp->collision), 0, barrel_trasform.sy, 0, barrel_trasform.sx, barrel_trasform.sy, barrel_trasform.sz);


    return &entity;
}


template <typename Comps, typename Tags>
void EntityFactory::createMap(uint8_t index)
{
    createMap_imp(Comps{}, Tags{}, index);
}

template <typename... Comps, typename... Tags>
void EntityFactory::createMap_imp(MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>, uint8_t index)
{
    std::ifstream               readFile;
    readFile.open(mapFiles[index]);

    try {
        // If finds the file, reads it
        if (!readFile.fail() && readFile.is_open()) {
            std::string line;

            // Initialize values
            size_t  nextPos;
            bool    nextData;
            std::vector<string>    data;
            std::vector<string>    coords, auxcoords;
            //Saves the actual model and material
            std::string actModel, actMaterial;

            while (readFile.good()) {
                // While there are lines to read, continues
                std::getline(readFile, line);

                // Split line for spaces
                data.clear();
                do {
                    //Search for model path
                    nextPos     = line.find(':');
                    nextData    = nextPos != line.npos;

                    if (nextData) {
                        data.emplace_back(line.substr(0, nextPos));
                        line = line.substr(nextPos+1);
                        data.emplace_back(line);
                    }
                } while(nextData);
                data.emplace_back(line);

                if(data.size() > 0) {
                    //If the line contains a object path process the data
                    if(data[0].compare("Model") == 0){
                        //Save the path for the actual model and material
                        actModel = modelFiles[index] + data[1];
                        std::string aux = data[1].substr(0, data[1].find_last_of("/"));
                        actMaterial= materialFiles[index] + aux.substr(0, aux.find(".")) + ".mtl";
                    }else if(data[0].compare("") != 0){
                        //Read the coordinates for the actual object to be loaded
                        coords.clear();
                        do {
                            //Split the position, rotation and scale
                            nextPos     = line.find('/');
                            nextData    = nextPos != line.npos;

                            if (nextData) {
                                coords.emplace_back(line.substr(0, nextPos));
                                line = line.substr(nextPos+1);
                                auxcoords.emplace_back(line);
                            }
                        } while(nextData);

                        coords.emplace_back(line);

                        MeshNode mesh;
                        //Search the material in the file system
                        ifstream f(actMaterial.c_str());
                        if(f.good()){
                            //Create a mesh with the model and material
                            mesh = createMesh(actModel.c_str(), actMaterial.c_str());
                        }else{
                            //Error: the file doesn't exists
                            std::string defaultTexture = "media/maps/map2/textures/TexturaPared.png";
                            mesh = createMesh(actModel.c_str(), defaultTexture.c_str());
                        }
                        //Change mesh transform with the data readed from the map
                        mesh.setPosition(std::stod(coords[0])*SCALE_FACTOR_MAP, std::stod(coords[2])*SCALE_FACTOR_MAP, std::stod(coords[1])*SCALE_FACTOR_MAP);
                        mesh.setRotation(-std::stod(coords[3]), -std::stod(coords[5]), -std::stod(coords[4]));
                        mesh.setScale(std::stod(coords[6])*SCALE_FACTOR_MAP, std::stod(coords[8])*SCALE_FACTOR_MAP, std::stod(coords[7])*SCALE_FACTOR_MAP);
                        //Add the mesh to the map
                        GameManager::Instance()->addMapMesh(mesh);
                    }
                    
                } 
            }
            //Close file
            readFile.close();
        }
    }catch (std::exception const& e){
        if(readFile.is_open())
            readFile.close();
    }
}

template <typename Comps, typename Tags>
void EntityFactory::deleteEnemyNormal(Entity& e)
{
    deleteEnemyNormal_imp(e, Comps{}, Tags{});
}

template <typename... Comps, typename... Tags>
void EntityFactory::deleteEnemyNormal_imp(Entity& e, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>)
{
    RenderComponent &rcmp = entity_man->template getComponent<RenderComponent>(e);
    //delete rcmp.node;
    CollisionComponent &ccmp = entity_man->template getComponent<CollisionComponent>(e);
    phy_engine->deleteCollisionObject(&ccmp.collision);

    AIComponent &aicmp = entity_man->template getComponent<AIComponent>(e);
    //delete aicmp.behaviourTree;
    aicmp.behaviourTree = nullptr;

    NavigationComponent &navcmp = entity_man->template getComponent<NavigationComponent>(e);
    navcmp.path.clear();

    entity_man->template removeComponent<BasicComponent>(e);
    (entity_man->template removeComponent<Comps>(e), ...);
    (entity_man->template removeTag<Tags>(e), ...);
    
    //Mark entity to destroy
    e.addTag<MarkToDestroyTag>();
    //entity_man->deleteEntity(e.getId());

    GameManager* gm = GameManager::Instance();
    gm->decreaseEnemy();
}

template <typename Comps, typename Tags>
Entity& EntityFactory::createBulletMachine(const Transform& entityTransform, const Transform& triggerTransform)
{
    return createBulletMachine_imp(entityTransform, triggerTransform, Comps{}, Tags{});
}

template <typename... Comps, typename... Tags>
Entity& EntityFactory::createBulletMachine_imp(const Transform& entityTransform, const Transform& triggerTransform, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>)
{

    Entity &entity = entity_man->createEntity(BasicComponent{   .x = entityTransform.x, .prevx = entityTransform.x, 
                                                                .y = entityTransform.y, .prevy = entityTransform.y, 
                                                                .z = entityTransform.z, .prevz = entityTransform.z,
                                                                .orientation = entityTransform.ry*PI/180, .prevorientation = entityTransform.ry*PI/180});

    ((void)entity_man->addComponent<Comps>(entity, Comps{}), ...);
    ((void)entity_man->addTag<Tags>(entity), ...);

    SoundComponent *s = &entity_man->getComponent<SoundComponent>(entity);
    SoundFactory* sFac = SoundFactory::Instance();

    s->idEvent.emplace_back(AMMO_MACHINE_GREETINGS);
    s->maskSounds.emplace_back(SOUND_3D);

    s->idEvent.emplace_back(AMMO_MACHINE_EXIT);
    s->maskSounds.emplace_back(SOUND_3D);

    s->idEvent.emplace_back(AMMO_MACHINE_MUSIC);
    s->maskSounds.emplace_back(SOUND_PLAY);
    s->maskSounds[s->maskSounds.size()-1] |= SOUND_3D;
    
    
    RenderComponent &rcomp2 = entity_man->template getComponent<RenderComponent>(entity);
    rcomp2.node = createMesh(MODEL_BULLET_MACHINE, TEXTURE_BULLET_MACHINE);
    rcomp2.node.setScale(SCALE_FACTOR_MAP, SCALE_FACTOR_MAP, SCALE_FACTOR_MAP);

    CollisionComponent *colCmp = &entity_man->template getComponent<CollisionComponent>(entity);
    phy_engine->addCollisionObject(colCmp, entity, SOLID_COLLISION);
    phy_engine->addBoxColliderToObject(&(colCmp->collision), 0, 10, 0, entityTransform.sx, entityTransform.sy, entityTransform.sz);

    TriggerComponent *tiggerCmp = &entity_man->template getComponent<TriggerComponent>(entity);
    phy_engine->addCollisionObject(tiggerCmp, entity, TRIGGER_COLLISION);
    phy_engine->addBoxColliderToObject(&(tiggerCmp->collision), triggerTransform.x, triggerTransform.y, triggerTransform.z, 
                                                                triggerTransform.sx, triggerTransform.sy, triggerTransform.sz);

    return entity;
}

template <typename Comps, typename Tags>
Entity& EntityFactory::createHealthMachine(const Transform& entityTransform, const Transform& triggerTransform)
{
    return createHealthMachine_imp(entityTransform, triggerTransform, Comps{}, Tags{});
}

template <typename... Comps, typename... Tags>
Entity& EntityFactory::createHealthMachine_imp(const Transform& entityTransform, const Transform& triggerTransform, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>)
{

    Entity &entity = entity_man->createEntity(BasicComponent{   .x = entityTransform.x, .prevx = entityTransform.x, 
                                                                .y = entityTransform.y, .prevy = entityTransform.y, 
                                                                .z = entityTransform.z, .prevz = entityTransform.z,
                                                                .orientation = entityTransform.ry*PI/180, .prevorientation = entityTransform.ry*PI/180});

    ((void)entity_man->addComponent<Comps>(entity, Comps{}), ...);
    ((void)entity_man->addTag<Tags>(entity), ...);

    SoundComponent *s = &entity_man->getComponent<SoundComponent>(entity);
    SoundFactory* sFac = SoundFactory::Instance();

    s->idEvent.emplace_back(HEAL_MACHINE_GREETINGS);
    s->maskSounds.emplace_back(SOUND_3D);

    s->idEvent.emplace_back(HEAL_MACHINE_EXIT);
    s->maskSounds.emplace_back(SOUND_3D);
    
    s->idEvent.emplace_back(HEAL_MACHINE_MUSIC);
    s->maskSounds.emplace_back(SOUND_PLAY);
    s->maskSounds[s->maskSounds.size()-1] |= SOUND_3D;

    RenderComponent &rcomp2 = entity_man->template getComponent<RenderComponent>(entity);
    rcomp2.node = createMesh(MODEL_HEALTH_MACHINE, TEXTURE_HEALTH_MACHINE);
    rcomp2.node.setScale(SCALE_FACTOR_MAP, SCALE_FACTOR_MAP, SCALE_FACTOR_MAP);

    CollisionComponent *colCmp = &entity_man->template getComponent<CollisionComponent>(entity);
    phy_engine->addCollisionObject(colCmp, entity, SOLID_COLLISION);
    phy_engine->addBoxColliderToObject(&(colCmp->collision), 0, 10, 0, entityTransform.sx, entityTransform.sy, entityTransform.sz);

    TriggerComponent *tiggerCmp = &entity_man->template getComponent<TriggerComponent>(entity);
    phy_engine->addCollisionObject(tiggerCmp, entity, TRIGGER_COLLISION);
    phy_engine->addBoxColliderToObject(&(tiggerCmp->collision), triggerTransform.x, triggerTransform.y, triggerTransform.z, 
                                                                triggerTransform.sx, triggerTransform.sy, triggerTransform.sz);

    return entity;
}

template <typename Comps, typename Tags>
Entity& EntityFactory::createEffectMachine(const Transform& entityTransform, const Transform& triggerTransform)
{
    return createEffectMachine_imp(entityTransform, triggerTransform, Comps{}, Tags{});
}

template <typename... Comps, typename... Tags>
Entity& EntityFactory::createEffectMachine_imp(const Transform& entityTransform, const Transform& triggerTransform, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>)
{

    Entity &entity = entity_man->createEntity(BasicComponent{   .x = entityTransform.x, .prevx = entityTransform.x, 
                                                                .y = entityTransform.y, .prevy = entityTransform.y, 
                                                                .z = entityTransform.z, .prevz = entityTransform.z,
                                                                .orientation = entityTransform.ry*PI/180, .prevorientation = entityTransform.ry*PI/180});

    ((void)entity_man->addComponent<Comps>(entity, Comps{}), ...);
    ((void)entity_man->addTag<Tags>(entity), ...);

    RenderComponent &rcomp2 = entity_man->template getComponent<RenderComponent>(entity);
    rcomp2.node = createMesh(MODEL_EFFECT_MACHINE, TEXTURE_EFFECT_MACHINE);
    rcomp2.node.setScale(SCALE_FACTOR_MAP, SCALE_FACTOR_MAP, SCALE_FACTOR_MAP);

    CollisionComponent *colCmp = &entity_man->template getComponent<CollisionComponent>(entity);
    phy_engine->addCollisionObject(colCmp, entity, SOLID_COLLISION);
    phy_engine->addBoxColliderToObject(&(colCmp->collision), 0, entityTransform.sy, 0, entityTransform.sx, entityTransform.sy, entityTransform.sz);

    TriggerComponent *tiggerCmp = &entity_man->template getComponent<TriggerComponent>(entity);
    phy_engine->addCollisionObject(tiggerCmp, entity, TRIGGER_COLLISION);
    phy_engine->addBoxColliderToObject(&(tiggerCmp->collision), triggerTransform.x, triggerTransform.y, triggerTransform.z, 
                                                                triggerTransform.sx, triggerTransform.sy, triggerTransform.sz);

    return entity;
}

template <typename Comps, typename Tags>
Entity& EntityFactory::createWeaponMachine(WeaponData weapon, const Transform& entityTransform, const Transform& triggerTransform)
{
    return createWeaponMachine_imp(weapon, entityTransform, triggerTransform, Comps{}, Tags{});
}

template <typename... Comps, typename... Tags>
Entity& EntityFactory::createWeaponMachine_imp(WeaponData weapon, const Transform& entityTransform, const Transform& triggerTransform, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>)
{

    Entity &entity = entity_man->createEntity(BasicComponent{   .x = entityTransform.x, .prevx = entityTransform.x, 
                                                                .y = entityTransform.y, .prevy = entityTransform.y, 
                                                                .z = entityTransform.z, .prevz = entityTransform.z,
                                                                .orientation = entityTransform.ry*PI/180, .prevorientation = entityTransform.ry*PI/180});

    ((void)entity_man->addComponent<Comps>(entity, Comps{}), ...);
    ((void)entity_man->addTag<Tags>(entity), ...);

    RenderComponent &rcomp2 = entity_man->template getComponent<RenderComponent>(entity);
    rcomp2.node = createMesh(WeaponTemplates::WEAPON_MACHINE_MODEL[weapon.weaponID-1].c_str(), WeaponTemplates::WEAPON_MACHINE_TEXTURE[weapon.weaponID-1].c_str());
    rcomp2.node.setScale(SCALE_FACTOR_MAP, SCALE_FACTOR_MAP, SCALE_FACTOR_MAP);

    CollisionComponent *colCmp = &entity_man->template getComponent<CollisionComponent>(entity);
    phy_engine->addCollisionObject(colCmp, entity, SOLID_COLLISION);
    phy_engine->addBoxColliderToObject(&(colCmp->collision), 0, 0, 0, entityTransform.sx, entityTransform.sy, entityTransform.sz);

    TriggerComponent *triggerCmp = &entity_man->template getComponent<TriggerComponent>(entity);
    triggerCmp->weaponData = weapon;
    phy_engine->addCollisionObject(triggerCmp, entity, TRIGGER_COLLISION);
    phy_engine->addBoxColliderToObject(&(triggerCmp->collision), triggerTransform.x, triggerTransform.y, triggerTransform.z, 
                                                                triggerTransform.sx, triggerTransform.sy, triggerTransform.sz);

    return entity;
}

template <typename Comps, typename Tags>
Entity& EntityFactory::createObject(const Transform& meshTrans, const Transform& colTrans, uint8_t pathIndex, size_t modelIndex) 
{
    return createObject_imp(meshTrans, colTrans, pathIndex, modelIndex, Comps{}, Tags{});
}

template <typename... Comps, typename... Tags>
Entity& EntityFactory::createObject_imp(const Transform& meshTrans, const Transform& colTrans, uint8_t pathIndex, size_t modelIndex, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>) {
    Entity &entity = entity_man->createEntity(BasicComponent{   .x = meshTrans.x, .prevx = meshTrans.x, 
                                                                .y = meshTrans.y, .prevy = meshTrans.y, 
                                                                .z = meshTrans.z, .prevz = meshTrans.z,
                                                                .orientation = meshTrans.ry*PI/180, .prevorientation = meshTrans.ry*PI/180});

    const std::vector<std::string> objectsNames{"EscombroPequeno",
                                                "PuertaMetal"       };

    ((void)entity_man->addComponent<Comps>(entity, Comps{}), ...);
    ((void)entity_man->addTag<Tags>(entity), ...);

    RenderComponent &rcomp = entity_man->template getComponent<RenderComponent>(entity);
    if(modelIndex< objectsNames.size())
        rcomp.node = createMesh((modelFiles[pathIndex] + objectsNames[modelIndex] + ".obj").c_str(), (materialFiles[pathIndex] + objectsNames[modelIndex] + ".mtl").c_str());
    else
        rcomp.node = createMesh((modelFiles[pathIndex] + objectsNames[0] + ".obj").c_str(), (materialFiles[pathIndex] + objectsNames[0] + ".mtl").c_str());
    rcomp.node.setScale(meshTrans.sx*SCALE_FACTOR_MAP, meshTrans.sy*SCALE_FACTOR_MAP, meshTrans.sz*SCALE_FACTOR_MAP);

    CollisionComponent *colCmp = &entity_man->template getComponent<CollisionComponent>(entity);
    phy_engine->addCollisionObject(colCmp, entity, SOLID_COLLISION);
    phy_engine->addBoxColliderToObject(&(colCmp->collision), colTrans.x, colTrans.y, colTrans.z, colTrans.sx, colTrans.sy, colTrans.sz);

    return entity;
}

template <typename Comps, typename Tags>
Entity& EntityFactory::createSoundEmitter(const Transform& transform, int soundId) 
{
    return createSoundEmitter_imp(transform, soundId, Comps{}, Tags{});
}

template <typename... Comps, typename... Tags>
Entity& EntityFactory::createSoundEmitter_imp(const Transform& transform, int soundId, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>) {
    Entity &entity = entity_man->createEntity(BasicComponent{   .x = transform.x, .prevx = transform.x, 
                                                                .y = transform.y, .prevy = transform.y, 
                                                                .z = transform.z, .prevz = transform.z  });

    ((void)entity_man->addComponent<Comps>(entity, Comps{}), ...);
    ((void)entity_man->addTag<Tags>(entity), ...);

    BasicComponent *basicCmp    = &entity_man->template getComponent<BasicComponent>(entity);
    SoundComponent *sound       = &entity_man->getComponent<SoundComponent>(entity);
    SoundFactory* factory       = SoundFactory::Instance();

    // Create sound
    sound->idEvent.emplace_back(soundId);
    sound->maskSounds.emplace_back(SOUND_PLAY | SOUND_NOT_RELEASE);
    sound->idInstance.emplace_back(factory->createInstanceFromEvent(soundId, true, Transform{std::vector<float>{basicCmp->x, basicCmp->y, basicCmp->z, (float)basicCmp->orientation, 0, (float)basicCmp->verticalOrientation, 0, 0, 0}}));

    return entity;
}

//Create wall door
template <typename Comps, typename Tags>
Entity& EntityFactory::createWallDoor(const Transform& entityTransform, const Transform& triggerTransform, int modelPosition)
{
    return createWallDoor_imp(entityTransform, triggerTransform, Comps{}, Tags{}, modelPosition);
}

template <typename... Comps, typename... Tags>
Entity& EntityFactory::createWallDoor_imp(const Transform& entityTransform, const Transform& triggerTransform, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>, int modelPosition)
{

    Entity &entity = entity_man->createEntity(BasicComponent{   .x = entityTransform.x, .prevx = entityTransform.x, 
                                                                .y = entityTransform.y, .prevy = entityTransform.y, 
                                                                .z = entityTransform.z, .prevz = entityTransform.z,
                                                                .orientation = entityTransform.ry*PI/180, .prevorientation = entityTransform.ry*PI/180});

    ((void)entity_man->addComponent<Comps>(entity, Comps{}), ...);
    ((void)entity_man->addTag<Tags>(entity), ...);

    RenderComponent &rcomp2 = entity_man->template getComponent<RenderComponent>(entity);
    rcomp2.node = createMesh(modelDoor[modelPosition].first.c_str(), modelDoor[modelPosition].second.c_str());
    rcomp2.node.setScale(SCALE_FACTOR_MAP, SCALE_FACTOR_MAP, SCALE_FACTOR_MAP);

    CollisionComponent *colCmp = &entity_man->template getComponent<CollisionComponent>(entity);
    phy_engine->addCollisionObject(colCmp, entity, SOLID_COLLISION);
    phy_engine->addBoxColliderToObject(&(colCmp->collision), 0, 0, 0, triggerTransform.sx/2, triggerTransform.sy, triggerTransform.sz);

    TriggerComponent *tiggerCmp = &entity_man->template getComponent<TriggerComponent>(entity);
    phy_engine->addCollisionObject(tiggerCmp, entity, TRIGGER_COLLISION);
    phy_engine->addBoxColliderToObject(&(tiggerCmp->collision), triggerTransform.x, triggerTransform.y, triggerTransform.z, 
                                                                triggerTransform.sx + 5.f, triggerTransform.sy, triggerTransform.sz);
    tiggerCmp->position = modelPosition;

    return entity;
}

//Create Trigger
template<typename Comps, typename Tags>
Entity& EntityFactory::createTrigger(const Transform& transform){
    return createTrigger_imp(transform, Comps{}, Tags{});
}

template<typename... Comps, typename... Tags>
Entity& EntityFactory::createTrigger_imp(const Transform& transform, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>){
    Entity &entity = entity_man->createEntity(BasicComponent{.x = transform.x, .prevx = transform.x, .y = transform.y, .prevy = transform.y, .z = transform.z, .prevz = transform.z});
    
    ((void)entity_man->addComponent<Comps>(entity, Comps{}), ...);
    ((void)entity_man->addTag<Tags>(entity), ...);

    TriggerComponent *tiggerCmp = &entity_man->template getComponent<TriggerComponent>(entity);
    phy_engine->addCollisionObject(tiggerCmp, entity, TRIGGER_COLLISION);
    phy_engine->addBoxColliderToObject(&(tiggerCmp->collision), 0, 0, 0, transform.sx, transform.sy, transform.sz);

    return entity;
}

template<typename Comps, typename Tags>
Entity& EntityFactory::createElectricPanel(const Transform& transform){
    return createElectricPanel_imp(transform, Comps{}, Tags{});
}

template<typename... Comps, typename... Tags>
Entity& EntityFactory::createElectricPanel_imp(const Transform& transform, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>){
    Entity &entity = entity_man->createEntity(BasicComponent{   .x = transform.x, .prevx = transform.x, 
                                                                .y = transform.y, .prevy = transform.y, 
                                                                .z = transform.z, .prevz = transform.z,
                                                                .orientation = transform.ry, .prevorientation = transform.ry});
    
    ((void)entity_man->addComponent<Comps>(entity, Comps{}), ...);
    ((void)entity_man->addTag<Tags>(entity), ...);
    entity.addTag<ElectricPanelTag>();

    RenderComponent *renderCmp = &entity_man->template getComponent<RenderComponent>(entity);
    renderCmp->node = createMesh((mapProps + "models/PanelElectrico.obj").c_str(), (mapProps + "materials/PanelElectrico.mtl").c_str());
    renderCmp->node.setScale(SCALE_FACTOR_MAP, SCALE_FACTOR_MAP, SCALE_FACTOR_MAP);

    HealthComponent* healthCmp = &entity_man->template getComponent<HealthComponent>(entity);
    healthCmp->health = 10.f;

    SoundComponent *s = &entity_man->getComponent<SoundComponent>(entity);
    SoundFactory* sFac = SoundFactory::Instance();
    BasicComponent *basicCmp = &entity_man->template getComponent<BasicComponent>(entity);

    s->idEvent.emplace_back(ELECTRIC_PANEL);
    s->maskSounds.emplace_back(SOUND_3D);
    s->maskSounds[s->maskSounds.size()-1] |= SOUND_NOT_RELEASE;
    s->idInstance.emplace_back(sFac->createInstanceFromEvent(ELECTRIC_PANEL, true, Transform{std::vector<float>{basicCmp->x, basicCmp->y, basicCmp->z, (float)basicCmp->orientation, 0, (float)basicCmp->verticalOrientation, 0, 0, 0}}));


    return entity;
}

template<typename Comps, typename Tags>
Entity& EntityFactory::createNestEntity(const Transform& transform){
    return createNestEntity_imp(transform, Comps{}, Tags{});
}

template<typename... Comps, typename... Tags>
Entity& EntityFactory::createNestEntity_imp(const Transform& transform, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>){
    Entity &entity = entity_man->createEntity(BasicComponent{   .x = transform.x, .prevx = transform.x, 
                                                                .y = transform.y, .prevy = transform.y, 
                                                                .z = transform.z, .prevz = transform.z,
                                                                .orientation = transform.ry, .prevorientation = transform.ry,
                                                                .verticalOrientation = transform.rx * PI/180});
    
    ((void)entity_man->addComponent<Comps>(entity, Comps{}), ...);
    ((void)entity_man->addTag<Tags>(entity), ...);

    RenderComponent *renderCmp = &entity_man->template getComponent<RenderComponent>(entity);
    renderCmp->node = createMesh((modelFiles[1] + "Nido.obj").c_str(), (materialFiles[1] + "Nido.mtl").c_str());
    renderCmp->node.setScale(SCALE_FACTOR_MAP, SCALE_FACTOR_MAP, SCALE_FACTOR_MAP);

    HealthComponent* healthCmp = &entity_man->template getComponent<HealthComponent>(entity);
    healthCmp->health = 180.f;

    return entity;
}

// Object destroyable by enemies
template<typename Comps, typename Tags>
Entity& EntityFactory::createEnemyObjetive(const Transform& transform){
    return createEnemyObjetive_imp(transform, Comps{}, Tags{});
}

template<typename... Comps, typename... Tags>
Entity& EntityFactory::createEnemyObjetive_imp(const Transform& transform, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>){
    Entity &entity = entity_man->createEntity(BasicComponent{   .x = transform.x, .prevx = transform.x,
                                                                .y = transform.y, .prevy = transform.y, 
                                                                .z = transform.z, .prevz = transform.z,
                                                                .orientation = transform.ry*PI/180, .prevorientation = transform.ry*PI/180});
    
    ((void)entity_man->addComponent<Comps>(entity, Comps{}), ...);
    ((void)entity_man->addTag<Tags>(entity), ...);

    RenderComponent &rcomp2 = entity_man->template getComponent<RenderComponent>(entity);
    rcomp2.node = createMesh(MODEL_ACTIVATE_AREA, TEXTURE_ACTIVATE_AREA);
    rcomp2.node.setScale(SCALE_FACTOR_MAP, SCALE_FACTOR_MAP, SCALE_FACTOR_MAP);

    CollisionComponent *colCmp = &entity_man->template getComponent<CollisionComponent>(entity);
    phy_engine->addCollisionObject(colCmp, entity, DESTROYABLE_COLLISION);
    phy_engine->addBoxColliderToObject(&(colCmp->collision), 0, 10, 0, 6.0f, 10.0f, 5.0f);

    TriggerComponent *tiggerCmp = &entity_man->template getComponent<TriggerComponent>(entity);
    phy_engine->addCollisionObject(tiggerCmp, entity, TRIGGER_COLLISION);
    phy_engine->addBoxColliderToObject(&(tiggerCmp->collision), 0, 5, 0, 30, 10, 30);

    std::unique_ptr<BehaviourTree> simpleEnemyBT (new BehaviourTree());

    auto * nodeCheckCurrentHp   = &simpleEnemyBT.get()->template createNode <BTCheckCurrentHealth>();
    auto * nodeIncreaseGoalTime = &simpleEnemyBT.get()->template createNode <BTIncreaseGoalTime>();

    simpleEnemyBT->template createNode <BTNodeSelector> (nodeCheckCurrentHp, nodeIncreaseGoalTime);

    AIComponent *aiCmp = &entity_man->template getComponent<AIComponent>(entity);
    aiCmp->behaviourTree = std::move(simpleEnemyBT);
    
    return entity;
}

// Object destroyable by enemies with movement
template<typename Comps, typename Tags>
Entity& EntityFactory::createMovableEnemyObjetive(const Transform& transform){
    return createMovableEnemyObjetive_imp(transform, Comps{}, Tags{});
}

template<typename... Comps, typename... Tags>
Entity& EntityFactory::createMovableEnemyObjetive_imp(const Transform& transform, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>){
    Entity &entity = entity_man->createEntity(BasicComponent{   .x = transform.x, .prevx = transform.x,
                                                                .y = transform.y, .prevy = transform.y, 
                                                                .z = transform.z, .prevz = transform.z,
                                                                .orientation = transform.ry*PI/180, .prevorientation = transform.ry*PI/180});
    
    ((void)entity_man->addComponent<Comps>(entity, Comps{}), ...);
    ((void)entity_man->addTag<Tags>(entity), ...);

    RenderComponent &rcomp2 = entity_man->template getComponent<RenderComponent>(entity);
    rcomp2.node = createMesh(MODEL_M1K3, TEXTURE_M1K3);
    rcomp2.node.setScale(SCALE_FACTOR_MAP, SCALE_FACTOR_MAP, SCALE_FACTOR_MAP);
    rcomp2.node.setVisible(false);

    std::unique_ptr<BehaviourTree> simpleEnemyBT (new BehaviourTree());

    auto * nodeCheckCurrentHp    = &simpleEnemyBT.get()->template createNode <BTCheckCurrentHealth>();

    auto * nodeCheckPlayerNearby = &simpleEnemyBT.get()->template createNode <BTPlayerNearby>();

    auto * nodeCheckCurrentPoint = &simpleEnemyBT.get()->template createNode <BTCheckRoutePoint>();

    auto * nodeCheckDoorFront    = &simpleEnemyBT.get()->template createNode <BTCheckDoorFront>();

    auto * nodeCalculateWp       = &simpleEnemyBT->template createNode <BTCalculateWaypoint>();
    auto * nodeArriveWp          = &simpleEnemyBT->template createNode <BTArriveToWaypoint>();
    auto * nodeSeqWp             = &simpleEnemyBT->template createNode <BTNodeSequence>(nodeCalculateWp, nodeArriveWp);

    simpleEnemyBT->template createNode <BTNodeSelector> (nodeCheckCurrentHp, nodeCheckPlayerNearby, nodeCheckCurrentPoint, nodeCheckDoorFront, nodeSeqWp);

    AIComponent *aiCmp = &entity_man->template getComponent<AIComponent>(entity);
    aiCmp->behaviourTree = std::move(simpleEnemyBT);
    aiCmp->routeToFollow = { {235, 10, 40, 0}, {340, 10, -350, 0}, {-80, 10, -530, 1} };
    aiCmp->arrivalRadius = 15;

    TriggerComponent *tiggerCmp = &entity_man->template getComponent<TriggerComponent>(entity);
    phy_engine->addCollisionObject(tiggerCmp, entity, TRIGGER_COLLISION);
    phy_engine->addBoxColliderToObject(&(tiggerCmp->collision), 0, 2, 0, 20, 10, 20);
    
    MovementComponent *movCmp = &entity_man->template getComponent<MovementComponent>(entity);
    movCmp->maxVelocity     = ROBOT_MAXVEL;
    movCmp->maxRunVelocity  = ROBOT_MAXVEL;
    movCmp->gravity         = 0;
    movCmp->maxGravity      = 0;

    SoundComponent *soundCmp = &entity_man->template getComponent<SoundComponent>(entity);
    SoundFactory* sFac = SoundFactory::Instance();
    BasicComponent *basicCmp = &entity_man->template getComponent<BasicComponent>(entity);

    soundCmp->idEvent.emplace_back(M1K3_FLY);
    soundCmp->maskSounds.emplace_back(SOUND_ALWAYS_UPDATE);
    soundCmp->maskSounds[soundCmp->maskSounds.size()-1] |= SOUND_NOT_RELEASE;
    soundCmp->idInstance.emplace_back(sFac->createInstanceFromEvent(M1K3_FLY, true, Transform{std::vector<float>{basicCmp->x, basicCmp->y, basicCmp->z, (float)basicCmp->orientation, 0, (float)basicCmp->verticalOrientation, 0, 0, 0}}));
    soundCmp->paramValues.insert_or_assign("M1K3estado", 0);
    return entity;
}