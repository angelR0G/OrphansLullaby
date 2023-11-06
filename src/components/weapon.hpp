#pragma once

struct WeaponData;

struct WeaponComponent{
	std::shared_ptr<WeaponData> firstWeapon{nullptr};
	std::shared_ptr<WeaponData> secondWeapon{nullptr};
	WeaponData* 				activeWeapon{nullptr};

	// Enemy hit
	float 	hitEnemy{};

	// Changing weapon
	// Change time
	float	changeWeaponTime{0.0};
	// Activate weapon change
	bool 	changeWeapon{false},
	 		getFirstWeapon{false},
			getSecondWeapon{false};

	// Recoil
	// Current recoil added
	float 	currentRecoil{0.0};
	// Objective recoil
	float	maxRecoilAdded{0.0};
	// Speed to recover aim
	float 	recoilReduction{0.0};

	// Idle animation variables
	float	weaponIdleX{0.0},
			weaponIdleY{0.0},
			weaponIdlePause{2.0};
	bool	weaponIdleIncrease{true};

	// Weapon movement interpolation
	float 	weaponYOffset{0.0f};

	~WeaponComponent() {}
};