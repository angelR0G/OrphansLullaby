#pragma once

#include "../IrrlichtFacade/mesh.hpp"
#include <optional>

// STATE FLAGS
#define WEAPON_DEFAULT		0	// Default state
#define WEAPON_FIRING 		1	// Pressing trigger to shot
#define WEAPON_SHOT			2	// Trigger not released since last shot
#define WEAPON_SINGLE_SHOT	4	// Single shot or automatic weapon
#define WEAPON_RELOAD		8	// Trying to reload
#define	WEAPON_ADD_RECOIL	16	// Moves camera to add recoil
#define	INFINITE_WEAPON		32	// This weapon has infinite ammo
#define	WEAPON_SHOTGUN		64	// Is a shotgun
#define	WEAPON_ROCKET		128	// Is a rocket launcher

#define MAX_WEAPON_LEVEL	10

struct WeaponData{
	// Weapon ID
	uint8_t weaponID{0};
	// Flags with different stats
	uint8_t weaponState{WEAPON_SINGLE_SHOT};
	// Shot damage
	float baseDamage{5};
	float damage{5};
	// Time between shots
    double  fireRate{0.2};
	// Time since last shot
	double	lastShot{0.0};
	// Current weapon ammo
	uint8_t	ammo{7};
	// Total ammo after reloading
	uint8_t magSize{7};
	// Current Total ammo
	uint16_t totalAmmo{20};
	// Total ammo
	uint16_t totalSize{50};
	// Current reloading time
	double 	reloadElapsed{2.5};
	// Total reloading time
	double 	reloadTime{2.5};
	// Recoil strength
	float	recoilForce{0.04};
	// Clamp max recoil
	float 	maxRecoilForce{0.8};
	// Upgrade level
	uint8_t	upgrades{0};


	// Weapon model
	std::optional<MeshNode> weaponNode;
	float 	weaponDistance{0.0};
	float 	weaponOffsetX{0.0},
			weaponOffsetY{0.0};
	float 	weaponDownOffsetX{0.0},
			weaponDownOffsetY{0.0};
};