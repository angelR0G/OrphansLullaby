#include <math.h>
#include <vector>

#include "../systems/navigation.hpp"

struct StressLevel;

struct RoundManager{
    RoundManager();
    void initializeRoundManager(NavigationSystem* ns);
    void spawnEnemies(float, float, float, float, EntityMan&, bool, StressLevel&);
    void calculateMaxEnemiesRound(bool);
    void decreaseEnemy();
    void respawnEnemy(int);
    void spawnNewEnemy(size_t, int);
    void increaseSpawnTime(float);
    void reset();

    void markLastRound() noexcept;

    int  getMaxEnemies();

    private:
        bool    spawn_enemy{};
        float   spawn_time{};
        int     round_num {}
            ,   round_rest {}              // numero de ronda dividido entre 5
            ,   max_enemies{}
            ,   dead_enemies{}
            ,   current_enemies{}
            ,   num_enemies_target_obj{}   // number of enemies targetting an objective
            ,   num_enemies_target_ply{};  // number of enemies targetting the player
        bool    lastRound{},
                lastRoundEnded{};

        uint8_t enemy_types_in_round {};
        std::vector <int> specials_spawns;

        // Pause timer between rounds
        float   pause_timer {};

        NavigationSystem* sys_nav {nullptr};
};