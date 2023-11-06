#pragma once

#include "../engine/types.hpp"
#include "../utils/collisionTypes.hpp"

struct weaponStats;
struct RayCast;
struct StressLevel;

struct WeaponSystem {
    void update(EntityMan& EM, PhysicsEngine& physics ,const double dt, weaponStats& wStats, StressLevel& stress);
    void reloadAmmo(EntityMan& EM, Entity& e);
    void deleteData(WeaponComponent&);
    void changeWeapon(Entity&, WeaponComponent*, bool, SoundComponent*);
    void changeWeapon(Entity&, WeaponComponent*, SoundComponent*);
    void changeAnimation(Entity&, WeaponComponent*);

    private:
        int checkShotCollision(RayCast*, Entity&, EntityMan&);
        int checkShotGunCollision(RayCast*, Entity&, EntityMan&, float, bool moreKillPoints = false);
        void createParticlesOnHit(Entity&, RayCast*, EntityMan&);
};