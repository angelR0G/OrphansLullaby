#pragma once

#include "../IrrlichtFacade/mesh.hpp"
#include "../engine/types.hpp"
#include "../utils/transform.hpp"
#include <cstdint>

struct GraphicEngine;

#define SCALE_FACTOR_MAP       10

#define MODEL_ENEMY_NORMAL      0
#define MODEL_ENEMY_EXPLOSIVE   1
#define MODEL_BULLET_MACHINE    2
#define MODEL_HEALTH_MACHINE    3
#define MODEL_DOOR_WALL         4
#define MODEL_ENEMY_RUGBY       5
#define MODEL_BARREL_EXP        6
#define MODEL_BARREL_SENY       7
#define MODEL_ENEMY_THROWER     8
#define MODEL_DEFEND_AREA       9
#define MODEL_EFFECT_MACHINE    10
#define MODEL_SPIT_ATTACK       11
#define MODEL_SLOW_ATTACK       12
#define MODEL_ROCKET_ATTACK     13
#define MODEL_ACTIVATE_AREA     14
#define MODEL_GPS               15
#define MODEL_M1K3              16
#define MODEL_GPS_ACTIVE_ZONE   17

#define TEXTURE_ENEMY_NORMAL    0
#define TEXTURE_ENEMY_EXPLOSIVE 1
#define TEXTURE_BULLET_MACHINE  2
#define TEXTURE_HEALTH_MACHINE  3
#define TEXTURE_DOOR_WALL       4
#define TEXTURE_ENEMY_RUGBY     5
#define TEXTURE_BARREL_EXP      6
#define TEXTURE_BARREL_TAR      7
#define TEXTURE_ENEMY_THROWER   8
#define TEXTURE_DEFEND_AREA     9
#define TEXTURE_EFFECT_MACHINE  10
#define TEXTURE_SPIT_ATTACK     11
#define TEXTURE_SLOW_ATTACK     12
#define TEXTURE_ROCKET_ATTACK   13
#define TEXTURE_ACTIVATE_AREA   14
#define TEXTURE_GPS             15
#define TEXTURE_M1K3            16
#define TEXTURE_GPS_ACTIVE_ZONE 17

#define ANIMATION_ENEMY_NORMAL_WALK     0
#define ANIMATION_ENEMY_EXPLOSIVE_WALK  1
#define ANIMATION_ENEMY_THROWER_WALK    2
#define ANIMATION_ENEMY_TANK_WALK       3

#define ANIMATION_ENEMY_NORMAL_ATTACK       0
#define ANIMATION_ENEMY_EXPLOSIVE_ATTACK    1
#define ANIMATION_ENEMY_THROWER_ATTACK      2
#define ANIMATION_ENEMY_TANK_ATTACK         3
#define ANIMATION_ENEMY_THROWER_SPIT_ATTACK 4
#define ANIMATION_ENEMY_TANK_SLOW_ATTACK    5

#define EXPLOSIVE_BARREL_ID     1
#define TARGET_BARREL_ID        2

/*  -------------------
    |  Barrel  stats  |
    -------------------  */
#define BARREL_HP                   1
#define EXPLOSIVE_BARREL_DURATION   30
#define TARGET_BARREL_DURATION      10
#define TARGET_BARREL_CD            30


/*  -------------------
    | Enemies attacks |
    -------------------  */
// Normal enemy
#define ENEMY_BASIC_AT_DURATION 0.35
#define ENEMY_BASIC_AT_COOLDOWN 0.90

// Explosive enemy
#define ENEMY_EX_AT_DURATION    2

// Rugby enemy
#define ENEMY_RUGBY_AT_DURATION         0.5
#define ENEMY_RUGBY_AT_COOLDOWN         2
#define ENEMY_RUGBY_AT_SLOW_DURATION    2
#define ENEMY_RUGBY_AT_SLOW_COOLDOWN    10

// Thrower enemy
#define ENEMY_THROWER_AT_DURATION         0.5
#define ENEMY_THROWER_AT_COOLDOWN         1
#define ENEMY_THROWER_AT_SPIT_DURATION    4
#define ENEMY_THROWER_AT_SPIT_COOLDOWN    3

/*  -----------------
    | Enemies stats |
    -----------------  */
// Common
    #define ENEMY_REGULAR_ACCEL         250.0

// Normal enemy
    #define NORMAL_ENEMY_DAMAGE         10.f
    #define NORMAL_ENEMY_HEALTH         10.f
    #define NORMAL_ENEMY_INFECTION      0.12f
    #define NORMAL_ENEMY_MAXVEL         32.f
    #define NORMAL_ENEMY_MAXRUNVEL      48.f

// Explosive enemy
    #define EXPLOSIVE_ENEMY_DAMAGE      30.f
    #define EXPLOSIVE_ENEMY_HEALTH      20.f
    #define EXPLOSIVE_ENEMY_INFECTION   0.6f
    #define EXPLOSIVE_ENEMY_MAXVEL      30.f
    #define EXPLOSIVE_ENEMY_MAXRUNVEL   45.f

// Rugby enemy
    #define RUGBY_ENEMY_DAMAGE          30.f
    #define RUGBY_ENEMY_DAMAGE_SLOW     20.f
    #define RUGBY_ENEMY_HEALTH          160.f
    #define RUGBY_ENEMY_INFECTION       0.2f
    #define RUGBY_ENEMY_MAXVEL          30.f
    #define RUGBY_ENEMY_MAXRUNVEL       45.f
    #define RUGBY_PROJECTILE_SPEED      50.f

// Thrower enemy
    #define THROWER_ENEMY_DAMAGE        7.5f
    #define THROWER_ENEMY_DAMAGE_SPIT   5.f
    #define THROWER_ENEMY_HEALTH        15.f
    #define THROWER_ENEMY_INFECTION     0.24f
    #define THROWER_ENEMY_MAXVEL        30.f
    #define THROWER_ENEMY_MAXRUNVEL     45.f
    #define THROWER_PROJECTILE_SPEED    75.f

// Rocket launcher
    #define ROCKET_PROJECTILE_SPEED     120.f

/*  -----------------
    |  Robot stats  |
    -----------------  */

    #define ROBOT_MAXVEL                15.f

class EntityFactory{

    public:
        static EntityFactory* Instance();
        ~EntityFactory();
        void initialize(EntityMan&, PhysicsEngine&, GraphicEngine&);
        MeshNode createMesh(uint16_t, uint16_t);
        MeshNode createMesh(const char*, const char*);
        Entity& initPlayer(const Transform&);
        Entity& initEnemyExplosive(float, float, float);
        Entity& initEnemyNormal(float, float, float, uint8_t);
        Entity& initEnemyRugby(float, float, float);
        Entity& initEnemyThrower(float, float, float);
        Entity* createEnemyAttack(BasicComponent*, std::vector<float>, uint16_t);
        Entity* createProjectileAttack(BasicComponent*, std::vector<float>, uint16_t, uint16_t);
        Entity* createExplosion(size_t, BasicComponent*, std::vector<float>, uint16_t);
        Entity* createBarrel(uint8_t, const Transform&);
        void initMap(uint8_t);
        
        void deleteEnemyNormal(Entity&);

        //Crete vending machine
        Entity& createBulletMachine(const Transform&, const Transform&);
        Entity& createHealthMachine(const Transform&, const Transform&);
        Entity& createEffectMachine(const Transform&, const Transform&);
        Entity& createWeaponMachine(WeaponData, const Transform&, const Transform&);
        
        Entity& createObject(const Transform&, const Transform&, uint8_t, size_t);
        Entity& createSoundEmitter(const Transform&, int);

        //Create door wall
        Entity& createWallDoor(const Transform&, const Transform&, int);

        // Create trigger
        Entity& createTrigger(const Transform&);

        // Create destroyable objects
        Entity& createElectricPanel(const Transform&);
        Entity& createNestEntity(const Transform&);

        Entity& createEnemyObjetive(const Transform&);
        Entity& createMovableEnemyObjetive(const Transform&);

        template<typename Comps, typename Tags>
        Entity& createPlayer(const Transform&);
        template<typename Comps, typename Tags>
        Entity& createEnemyNormal(float, float, float, uint8_t);
        template<typename Comps, typename Tags>
        Entity& createEnemyExplosive(float, float, float);
        template<typename Comps, typename Tags>
        Entity& createEnemyRugby(float, float, float);
        template<typename Comps, typename Tags>
        Entity& createEnemyThrower(float, float, float);
        template<typename Comps, typename Tags>
        Entity* createEnemyAttack(BasicComponent*, std::vector<float>, uint16_t);
        template<typename Comps, typename Tags>
        Entity* createProjectileAttack(BasicComponent*, std::vector<float>, uint16_t, uint16_t);
        template<typename Comps, typename Tags>
        Entity* createExplosion(size_t, BasicComponent*, std::vector<float>, uint16_t);
        
        template<typename Comps, typename Tags>
        Entity* createBarrel(uint8_t, const Transform&);
        
        template<typename Comps, typename Tags>
        void createMap(uint8_t);
        template<typename Comps, typename Tags>
        void deleteEnemyNormal(Entity&);
        template<typename Comps, typename Tags>
        Entity& createObject(const Transform&, const Transform&, uint8_t, size_t);
        template<typename Comps, typename Tags>
        Entity& createSoundEmitter(const Transform&, int);

        //Crete vending machine
        template<typename Comps, typename Tags>
        Entity& createBulletMachine(const Transform&, const Transform&);
        template<typename Comps, typename Tags>
        Entity& createHealthMachine(const Transform&, const Transform&);
        template<typename Comps, typename Tags>
        Entity& createEffectMachine(const Transform&, const Transform&);
        template<typename Comps, typename Tags>
        Entity& createWeaponMachine(WeaponData, const Transform&, const Transform&);
        
        //Create door wall
        template<typename Comps, typename Tags>
        Entity& createWallDoor(const Transform&, const Transform&, int);

        //Create triggers
        template<typename Comps, typename Tags>
        Entity& createTrigger(const Transform&);

        //Create destroyable objects
        template<typename Comps, typename Tags>
        Entity& createElectricPanel(const Transform&);
        template<typename Comps, typename Tags>
        Entity& createNestEntity(const Transform&);

        //Create object destroyable by enemies
        template<typename Comps, typename Tags>
        Entity& createEnemyObjetive(const Transform&);

        //Create object destroyable by enemies
        template<typename Comps, typename Tags>
        Entity& createMovableEnemyObjetive(const Transform&);

        void checkDeleteEnemy(Entity&);
        void createAnimation(Entity&, int, std::string, bool, bool, bool);
        void clearAnimations(Entity&);
        void preloadWeaponsAndEnemies();
    private:
        EntityFactory();

        template<typename... Comps, typename... Tags>
        Entity& createPlayer_imp(MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>, const Transform&);
        template<typename... Comps, typename... Tags>
        Entity& createEnemyNormal_imp(MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>, float, float, float, uint8_t);
        template<typename... Comps, typename... Tags>
        Entity& createEnemyExplosive_imp(MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>, float, float, float);
        template<typename... Comps, typename... Tags>
        Entity& createEnemyRugby_imp(MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>, float, float, float);
        template<typename... Comps, typename... Tags>
        Entity& createEnemyThrower_imp(MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>, float, float, float);
        template<typename... Comps, typename... Tags>
        void createMap_imp(MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>, uint8_t);
        template<typename... Comps, typename... Tags>
        void deleteEnemyNormal_imp(Entity& ,MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>);
        template<typename... Comps, typename... Tags>
        Entity* createEnemyAttack_imp(MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>,BasicComponent*, std::vector<float>, uint16_t);
        template<typename... Comps, typename... Tags>
        Entity* createProjectileAttack_imp(MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>,BasicComponent*, std::vector<float>, uint16_t, uint16_t);
        template<typename... Comps, typename... Tags>
        Entity* createExplosion_imp(MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>, size_t, BasicComponent*, std::vector<float>, uint16_t);

        template<typename... Comps, typename... Tags>
        Entity* createBarrel_imp(uint8_t, const Transform&, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>);

        template<typename... Comps, typename... Tags>
        Entity& createBulletMachine_imp(const Transform&, const Transform&, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>);
        template<typename... Comps, typename... Tags>
        Entity& createHealthMachine_imp(const Transform&, const Transform&, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>);
        template<typename... Comps, typename... Tags>
        Entity& createEffectMachine_imp(const Transform&, const Transform&, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>);
        template<typename... Comps, typename... Tags>
        Entity& createWeaponMachine_imp(WeaponData, const Transform&, const Transform&, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>);

        template<typename... Comps, typename... Tags>
        Entity& createWallDoor_imp(const Transform&, const Transform&, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>, int);

        template<typename... Comps, typename... Tags>
        Entity& createObject_imp(const Transform&, const Transform&, uint8_t, size_t, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>);
        template<typename... Comps, typename... Tags>
        Entity& createSoundEmitter_imp(const Transform&, int, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>);
        
        //Create triggers
        template<typename... Comps, typename... Tags>
        Entity& createTrigger_imp(const Transform&, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>);

        //Create destroyable objects
        template<typename... Comps, typename... Tags>
        Entity& createElectricPanel_imp(const Transform&, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>);
        template<typename... Comps, typename... Tags>
        Entity& createNestEntity_imp(const Transform&, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>);

        //Create object destroyable by enemies
        template<typename... Comps, typename... Tags>
        Entity& createEnemyObjetive_imp(const Transform&, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>);

        //Create object destroyable by enemies
        template<typename... Comps, typename... Tags>
        Entity& createMovableEnemyObjetive_imp(const Transform&, MetaP::Typelist<Comps...>, MetaP::Typelist<Tags...>);

        //Member variables
        //std::vector<std::shared_ptr<MeshNode>> nodePtr{};
        EntityMan* entity_man{};
        PhysicsEngine* phy_engine{};
        GraphicEngine* engine{};
        
        inline static std::unique_ptr<EntityFactory> pEntityFactory{nullptr};

};