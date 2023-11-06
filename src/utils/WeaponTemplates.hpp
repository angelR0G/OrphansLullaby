#pragma once

#include "WeaponData.hpp"
#include <vector>
#include <string>

#define MAP_PROPS_MEDIA_PATH(file) "media/maps/map_props/" file

#define WTEMPLATE_ID_BASIC_GUN          0
#define WTEMPLATE_ID_REVOLVER           1
#define WTEMPLATE_ID_AUTOMATIC_GUN      2
#define WTEMPLATE_ID_SUBMACHINE_GUN     3
#define WTEMPLATE_ID_ASSAULT_RIFLE      4
#define WTEMPLATE_ID_RIFLE              5
#define WTEMPLATE_ID_RAY_GUN            6
#define WTEMPLATE_ID_COMBAT_SHOTGUN     7
#define WTEMPLATE_ID_SHOTGUN            8
#define WTEMPLATE_ID_MACHINE_GUN        9
#define WTEMPLATE_ID_BAZOOKA            10

namespace WeaponTemplates {

    //Weapon models paths macros
    const std::string weaponPathModel       = "media/weapons/models/";
    const std::string weaponPathMaterials   = "media/weapons/materials/";
    const std::string weaponMachineMaterials   = "media/maps/map_props/";
    #define WEAPON_MODEL_MEDIA_PATH(file)       weaponPathModel     + file
    #define WEAPON_MATERIALS_MEDIA_PATH(file)   weaponPathMaterials + file
    #define WEAPON_MACHINE_MEDIA_PATH(file)     weaponMachineMaterials + file

    //Weapons models
    const static std::vector<std::string> WEAPON_MODEL{   
        WEAPON_MODEL_MEDIA_PATH("pistola_disparoModel.fbx"), 
        WEAPON_MODEL_MEDIA_PATH("revolver_disparoModel.fbx"), 
        WEAPON_MODEL_MEDIA_PATH("pistola_automatica_disparoModel.fbx"), 
        WEAPON_MODEL_MEDIA_PATH("subfusil_disparoModel.fbx"),
        WEAPON_MODEL_MEDIA_PATH("m16_disparoModel.fbx"), 
        WEAPON_MODEL_MEDIA_PATH("m1_disparoModel.fbx"), 
        WEAPON_MODEL_MEDIA_PATH("rayos_disparoModel.fbx"), 
        WEAPON_MODEL_MEDIA_PATH("tact_shotgun_disparoModel.fbx"), 
        WEAPON_MODEL_MEDIA_PATH("escopeta_disparoModel.fbx"), 
        WEAPON_MODEL_MEDIA_PATH("ametralladora_disparoModel.fbx"),
        WEAPON_MODEL_MEDIA_PATH("lanzacohetes_disparoModel.fbx")
    };

    const static std::vector<std::string> WEAPON_TEXTURE{ 
        WEAPON_MATERIALS_MEDIA_PATH("Brazos_Pistola.mtl"), 
        WEAPON_MATERIALS_MEDIA_PATH("brazos_revolver.mtl"), 
        WEAPON_MATERIALS_MEDIA_PATH("brazos_pistolaautomatica.mtl"), 
        WEAPON_MATERIALS_MEDIA_PATH("Brazos_Subfusil.mtl"),
        WEAPON_MATERIALS_MEDIA_PATH("Brazos_M16.mtl"), 
        WEAPON_MATERIALS_MEDIA_PATH("brazos_m1.mtl"), 
        WEAPON_MATERIALS_MEDIA_PATH("rayos_disparo.mtl"), 
        WEAPON_MATERIALS_MEDIA_PATH("Brazos_TactShotgun.mtl"), 
        WEAPON_MATERIALS_MEDIA_PATH("Brazos_Escopeta.mtl"), 
        WEAPON_MATERIALS_MEDIA_PATH("brazos_ametralladora.mtl"),
        WEAPON_MATERIALS_MEDIA_PATH("Brazos_lanzacohetes.mtl")
    };

    const static std::vector<std::string> WEAPON_IDLE_ANIM{
        WEAPON_MODEL_MEDIA_PATH("pistola_estatico.fbx"),
        WEAPON_MODEL_MEDIA_PATH("revolver_estatico.fbx"),
        WEAPON_MODEL_MEDIA_PATH("pistola_automatica_estatico.fbx"),
        WEAPON_MODEL_MEDIA_PATH("subfusil_estatico.fbx"),
        WEAPON_MODEL_MEDIA_PATH("m16_estatico.fbx"),
        WEAPON_MODEL_MEDIA_PATH("m1_estatico.fbx"),
        WEAPON_MODEL_MEDIA_PATH("rayos_estatico.fbx"),
        WEAPON_MODEL_MEDIA_PATH("tact_shotgun_estatico.fbx"),
        WEAPON_MODEL_MEDIA_PATH("escopeta_estatico.fbx"),
        WEAPON_MODEL_MEDIA_PATH("ametralladora_estatico.fbx"),
        WEAPON_MODEL_MEDIA_PATH("lanzacohetes_estatico.fbx"),
    };

    const static std::vector<std::string> WEAPON_SHOT_ANIM{
        WEAPON_MODEL_MEDIA_PATH("pistola_disparo.fbx"),
        WEAPON_MODEL_MEDIA_PATH("revolver_disparo.fbx"),
        WEAPON_MODEL_MEDIA_PATH("pistola_automatica_disparo.fbx"),
        WEAPON_MODEL_MEDIA_PATH("subfusil_disparo.fbx"),
        WEAPON_MODEL_MEDIA_PATH("m16_disparo.fbx"),
        WEAPON_MODEL_MEDIA_PATH("m1_disparo.fbx"),
        WEAPON_MODEL_MEDIA_PATH("rayos_disparo.fbx"),
        WEAPON_MODEL_MEDIA_PATH("tact_shotgun_disparo.fbx"),
        WEAPON_MODEL_MEDIA_PATH("escopeta_disparo.fbx"),
        WEAPON_MODEL_MEDIA_PATH("ametralladora_disparo.fbx"),
        WEAPON_MODEL_MEDIA_PATH("lanzacohetes_disparo.fbx"),
    };

    const static std::string RIFLE_LAST_SHOT_ANIM = WEAPON_MODEL_MEDIA_PATH("m1_ultimo_tiro.fbx");
    const static std::string TSHOTGUN_FIRST_RELOAD_ANIM = WEAPON_MODEL_MEDIA_PATH("tact_shotgun_primera_bala.fbx");
    const static std::string TSHOTGUN_LAST_RELOAD_ANIM = WEAPON_MODEL_MEDIA_PATH("tact_shotgun_ultima_bala.fbx");

    const static std::vector<std::string> WEAPON_RELOAD_ANIM{
        WEAPON_MODEL_MEDIA_PATH("pistola_recarga.fbx"),
        WEAPON_MODEL_MEDIA_PATH("revolver_recarga.fbx"),
        WEAPON_MODEL_MEDIA_PATH("pistola_automatica_recarga.fbx"),
        WEAPON_MODEL_MEDIA_PATH("subfusil_recarga.fbx"),
        WEAPON_MODEL_MEDIA_PATH("m16_recarga.fbx"),
        WEAPON_MODEL_MEDIA_PATH("m1_recarga.fbx"),
        WEAPON_MODEL_MEDIA_PATH("rayos_recarga.fbx"),
        WEAPON_MODEL_MEDIA_PATH("tact_shotgun_recarga.fbx"),
        WEAPON_MODEL_MEDIA_PATH("escopeta_recarga.fbx"),
        WEAPON_MODEL_MEDIA_PATH("ametralladora_recarga.fbx"),
        WEAPON_MODEL_MEDIA_PATH("lanzacohetes_recarga.fbx"),
    };

    const static std::vector<std::string> WEAPON_MACHINE_MODEL = { 
        WEAPON_MACHINE_MEDIA_PATH("models/weaponmachinePistol.obj"), 
        WEAPON_MACHINE_MEDIA_PATH("models/weaponmachineRevolver.obj"), 
        WEAPON_MACHINE_MEDIA_PATH("models/weaponmachineAutomaticPistol.obj"), 
        WEAPON_MACHINE_MEDIA_PATH("models/weaponmachineSubMachineGun.obj"), 
        WEAPON_MACHINE_MEDIA_PATH("models/weaponmachineAssaultRifle.obj"), 
        WEAPON_MACHINE_MEDIA_PATH("models/weaponmachineCarabina.obj"), 
        WEAPON_MACHINE_MEDIA_PATH("models/weaponmachineRayGun.obj"),
        WEAPON_MACHINE_MEDIA_PATH("models/weaponmachineCombatShotgun.obj"), 
        WEAPON_MACHINE_MEDIA_PATH("models/weaponmachineShotgun.obj"), 
        WEAPON_MACHINE_MEDIA_PATH("models/weaponmachineMachineGun.obj"),
        WEAPON_MACHINE_MEDIA_PATH("models/weaponmachineBazooka.obj")
    };
    
    const static std::vector<std::string> WEAPON_MACHINE_TEXTURE{ 
        WEAPON_MACHINE_MEDIA_PATH("materials/weaponMachinePistol.mtl"), 
        WEAPON_MACHINE_MEDIA_PATH("materials/weaponMachineRevolver.mtl"), 
        WEAPON_MACHINE_MEDIA_PATH("materials/weaponMachineAutomaticPistol.mtl"), 
        WEAPON_MACHINE_MEDIA_PATH("materials/weaponMachineSubMachineGun.mtl"), 
        WEAPON_MACHINE_MEDIA_PATH("materials/weaponMachineAssaultRifle.mtl"), 
        WEAPON_MACHINE_MEDIA_PATH("materials/weaponMachineCarabina.mtl"), 
        WEAPON_MACHINE_MEDIA_PATH("materials/weaponMachineRayGun.mtl"),
        WEAPON_MACHINE_MEDIA_PATH("materials/weaponMachineCombatShotgun.mtl"), 
        WEAPON_MACHINE_MEDIA_PATH("materials/weaponMachineShotgun.mtl"), 
        WEAPON_MACHINE_MEDIA_PATH("materials/weaponMachineMachineGun.mtl"),
        WEAPON_MACHINE_MEDIA_PATH("materials/weaponMachineBazooka.mtl")
    };

    const static std::vector<WeaponData> weapons {
    // Basic gun
    {
        1,
        WEAPON_SINGLE_SHOT | INFINITE_WEAPON,   // weaponState
        3.1,        // baseDamage
        3.1,        // damage
        0.18,       // fireRate
        0.0,        // lastShot
        14,         // ammo
        14,         // magSize
        1400,       // totalAmmo
        1400,       // totalSize
        2.5,        // reloadElapsed
        2.5,        // reloadTime
        0.03,       // recoilForce
        0.06,       // maxRecoilForce
        0,          // upgradeLevel
        std::nullopt,    // weaponNode
        2.0,        // weaponDistance
        2.0,        // weaponOffsetX
        -1.0,       // weaponOffsetY
        1.0,        // weaponDownOffsetX
        -2.5        // weaponDownOffsetY
    },

    // Revolver
    {
        2,
        WEAPON_SINGLE_SHOT | INFINITE_WEAPON,   // weaponState
        7.4,        // baseDamage
        7.4,        // damage
        0.45,       // fireRate
        0.0,        // lastShot
        6,          // ammo
        6,          // magSize
        600,        // totalAmmo
        600,        // totalSize
        2.5,        // reloadElapsed
        2.5,        // reloadTime
        0.06,       // recoilForce
        0.1,        // maxRecoilForce
        0,          // upgradeLevel
        std::nullopt,    // weaponNode
        2.0,        // weaponDistance
        2.0,        // weaponOffsetX
        -1.0,       // weaponOffsetY
        1.0,        // weaponDownOffsetX
        -2.5        // weaponDownOffsetY
    },

    // Automatic gun
    {
        3,
        INFINITE_WEAPON,   // weaponState
        1.5,        // baseDamage
        1.5,        // damage
        0.1,        // fireRate
        0.0,        // lastShot
        18,         // ammo
        18,         // magSize
        1800,       // totalAmmo
        1800,       // totalSize
        2.5,        // reloadElapsed
        2.5,        // reloadTime
        0.02,       // recoilForce
        0.05,       // maxRecoilForce
        0,          // upgradeLevel
        std::nullopt,    // weaponNode
        2.0,        // weaponDistance
        2.0,        // weaponOffsetX
        -1.0,       // weaponOffsetY
        1.0,        // weaponDownOffsetX
        -2.5        // weaponDownOffsetY
    },

    // Sub machine gun
    {
        4,
        WEAPON_DEFAULT,     // weaponState
        2.4,        // baseDamage
        2.4,        // damage
        0.07,       // fireRate
        0.0,        // lastShot
        36,         // ammo
        36,         // magSize
        480,        // totalAmmo
        480,        // totalSize
        2.5,        // reloadElapsed
        2.5,        // reloadTime
        0.02,       // recoilForce
        0.11,       // maxRecoilForce
        0,          // upgradeLevel
        std::nullopt,    // weaponNode
        5.0,        // weaponDistance
        2.0,        // weaponOffsetX
        -1.5,       // weaponOffsetY
        1.0,        // weaponDownOffsetX
        -2.5        // weaponDownOffsetY
    },

    // Assault rifle
    {
        5,
        WEAPON_DEFAULT,     // weaponState
        5.0,        // baseDamage
        5.0,        // damage
        0.14,       // fireRate
        0.0,        // lastShot
        24,         // ammo
        24,         // magSize
        360,        // totalAmmo
        360,        // totalSize
        2.5,        // reloadElapsed
        2.5,        // reloadTime
        0.04,       // recoilForce
        0.16,       // maxRecoilForce
        0,          // upgradeLevel
        std::nullopt,    // weaponNode
        2.0,        // weaponDistance
        2.0,        // weaponOffsetX
        -1.0,       // weaponOffsetY
        1.0,        // weaponDownOffsetX
        -2.5        // weaponDownOffsetY
    },

    // Rifle
    {
        6,
        WEAPON_SINGLE_SHOT,     // weaponState
        6.8,        // baseDamage
        6.8,        // damage
        0.38,       // fireRate
        0.0,        // lastShot
        12,         // ammo
        12,         // magSize
        100,        // totalAmmo
        100,        // totalSize
        2.5,        // reloadElapsed
        2.5,        // reloadTime
        0.03,       // recoilForce
        0.03,       // maxRecoilForce
        0,          // upgradeLevel
        std::nullopt,    // weaponNode
        2.0,        // weaponDistance
        2.0,        // weaponOffsetX
        -1.0,       // weaponOffsetY
        1.0,        // weaponDownOffsetX
        -2.5        // weaponDownOffsetY
    },

    // Ray gun
    {
        7,
        WEAPON_SINGLE_SHOT,     // weaponState
        8.0,        // baseDamage
        8.0,        // damage
        0.48,        // fireRate
        0.0,        // lastShot
        10,         // ammo
        10,         // magSize
        100,        // totalAmmo
        100,        // totalSize
        2.0,        // reloadElapsed
        2.0,        // reloadTime
        0.03,       // recoilForce
        0.03,       // maxRecoilForce
        0,          // upgradeLevel
        std::nullopt,    // weaponNode
        2.0,        // weaponDistance
        2.0,        // weaponOffsetX
        -1.0,       // weaponOffsetY
        1.0,        // weaponDownOffsetX
        -2.5        // weaponDownOffsetY
    },

    // Combat shotgun
    {
        8,
        WEAPON_SINGLE_SHOT | WEAPON_SHOTGUN,    // weaponState
        2.0,        // baseDamage
        2.0,        // damage
        0.9,        // fireRate
        0.0,        // lastShot
        6,          // ammo
        6,          // magSize
        80,         // totalAmmo
        80,         // totalSize
        5,          // reloadElapsed
        5,          // reloadTime
        0.07,       // recoilForce
        0.07,       // maxRecoilForce
        0,          // upgradeLevel
        std::nullopt,    // weaponNode
        2.0,        // weaponDistance
        2.0,        // weaponOffsetX
        -1.0,       // weaponOffsetY
        1.0,        // weaponDownOffsetX
        -2.5        // weaponDownOffsetY
    },

    // Shotgun
    {
        9,
        WEAPON_SINGLE_SHOT | WEAPON_SHOTGUN,    // weaponState
        2.8,        // baseDamage
        2.8,        // damage
        0.5,        // fireRate
        0.0,        // lastShot
        2,          // ammo
        2,          // magSize
        60,         // totalAmmo
        60,         // totalSize
        2.5,        // reloadElapsed
        2.5,        // reloadTime
        0.12,       // recoilForce
        0.18,       // maxRecoilForce
        0,          // upgradeLevel
        std::nullopt,    // weaponNode
        2.0,        // weaponDistance
        2.0,        // weaponOffsetX
        -1.0,       // weaponOffsetY
        1.0,        // weaponDownOffsetX
        -2.5        // weaponDownOffsetY
    },

    // Machine gun
    {
        10,
        WEAPON_DEFAULT,     // weaponState
        3.5,        // baseDamage
        3.5,        // damage
        0.17,       // fireRate
        0.0,        // lastShot
        80,         // ammo
        80,         // magSize
        600,        // totalAmmo
        600,        // totalSize
        2.5,        // reloadElapsed
        4.5,        // reloadTime
        0.036,      // recoilForce
        0.2,        // maxRecoilForce
        0,          // upgradeLevel
        std::nullopt,    // weaponNode
        2.0,        // weaponDistance
        2.0,        // weaponOffsetX
        -1.0,       // weaponOffsetY
        1.0,        // weaponDownOffsetX
        -2.5        // weaponDownOffsetY
    },

    // Bazooka
    {
        11,
        WEAPON_ROCKET,      // weaponState
        50.0,       // baseDamage
        50.0,       // damage
        0.1,        // fireRate
        0.0,        // lastShot
        1,          // ammo
        1,          // magSize
        15,         // totalAmmo
        15,         // totalSize
        2.5,        // reloadElapsed
        2.5,        // reloadTime
        0.08,       // recoilForce
        0.08,       // maxRecoilForce
        0,          // upgradeLevel
        std::nullopt,    // weaponNode
        2.0,        // weaponDistance
        2.0,        // weaponOffsetX
        -1.0,       // weaponOffsetY
        1.0,        // weaponDownOffsetX
        -3.0        // weaponDownOffsetY
    }
    };
}