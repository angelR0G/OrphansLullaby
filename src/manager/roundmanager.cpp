#include "roundmanager.hpp"

#include "../utils/waypoint.hpp"
#include "../utils/stressLevel.hpp"

#include "../engine/entityFactory.hpp"

#include "../Fmod/MasksPositions.hpp"

#include "eventmanager.hpp"

#define TIME_BETWEEN_ROUNDS     8

#define INITIAL_ENEMIES         5
#define ROUND_DIVISOR           2
#define ENEMIES_INCREASE_CONST  2   // Const to increase the enemie rand

#define NORMAL_ENEMY_MASK       1
#define EXPLOSIVE_ENEMY_MASK    2
#define THROWER_ENEMY_MASK      4
#define RUGBY_ENEMY_MASK        8

#define EXPLOSIVE_ENEMY_ROUND   2
#define THROWER_ENEMY_ROUND     4
#define RUGBY_ENEMY_ROUND       6

#define EXPLOSIVE_VECTOR_POS    0
#define THROWER_VECTOR_POS      1
#define RUGBY_VECTOR_POS        2

// enemies target masks
#define PLAYER_TARGET_MASK    0
#define OBJECTIVE_TARGET_MASK 1

const float TIME_SPAWN        = 0.6f;

RoundManager::RoundManager() {
    reset();
}

void RoundManager::initializeRoundManager(NavigationSystem* ns){
    sys_nav = ns;
}

void RoundManager::calculateMaxEnemiesRound(bool isDefendingObjectiveActive){
    round_num++;
    EventManager* eventMan = EventManager::Instance();
    std::shared_ptr<Event> event ( new Event(EVENT_ROUND) );
    
    eventMan->scheduleEvent(event);

    int new_round_rest = round_num/ROUND_DIVISOR;

    // Calculate enemies amount per round
    max_enemies = INITIAL_ENEMIES + new_round_rest * ENEMIES_INCREASE_CONST;

    if(round_num==EXPLOSIVE_ENEMY_ROUND){
        enemy_types_in_round += EXPLOSIVE_ENEMY_MASK;
        specials_spawns[EXPLOSIVE_VECTOR_POS] = std::rand()%(max_enemies-1);
    }

    if(round_num==THROWER_ENEMY_ROUND){
        enemy_types_in_round += THROWER_ENEMY_MASK;
        specials_spawns[THROWER_VECTOR_POS] = std::rand()%(max_enemies-1);
    }

    if(round_num==RUGBY_ENEMY_ROUND){
        enemy_types_in_round += RUGBY_ENEMY_MASK;
        specials_spawns[RUGBY_VECTOR_POS] = std::rand()%(max_enemies-1);
    }

    if(round_num>EXPLOSIVE_ENEMY_ROUND){
        if(std::rand()%9>4){
            specials_spawns[EXPLOSIVE_VECTOR_POS] = std::rand()%(max_enemies-1);
        }
    }
    if(round_num>THROWER_ENEMY_ROUND){
        if(std::rand()%9>2){
            do{
                specials_spawns[THROWER_VECTOR_POS] = std::rand()%(max_enemies-1);
            }while(specials_spawns[THROWER_VECTOR_POS]==specials_spawns[EXPLOSIVE_VECTOR_POS]);
        }
    }
    if(round_num>RUGBY_ENEMY_ROUND){
        if(std::rand()%9>6){
            do{
                specials_spawns[RUGBY_VECTOR_POS] = std::rand()%(max_enemies-1);
            }while(specials_spawns[RUGBY_VECTOR_POS]==specials_spawns[EXPLOSIVE_VECTOR_POS] || specials_spawns[RUGBY_VECTOR_POS]==specials_spawns[THROWER_VECTOR_POS]);
        }
    }

    if(isDefendingObjectiveActive){
        int specials_enemies_in_round {0};
        for(unsigned int i=0; i<specials_spawns.size(); i++){
            if(specials_spawns[i]!=-1){
                specials_enemies_in_round++;
            }
        }

        int max_normal_enemies = max_enemies - specials_enemies_in_round;
        num_enemies_target_obj = max_normal_enemies/2;
        num_enemies_target_ply = max_normal_enemies - num_enemies_target_obj;
    }

}

void RoundManager::increaseSpawnTime(float time){
    spawn_time += time;
}

void RoundManager::decreaseEnemy(){
    dead_enemies++;
}

void RoundManager::respawnEnemy(int type){
    --current_enemies;
    spawn_enemy = true;

    // Check type of enemy
    if(type > 0) {
        specials_spawns[type-1] = current_enemies;
    }
}

void RoundManager::spawnNewEnemy(size_t number, int type){
    max_enemies += number;
    spawn_enemy = true;

    // Check type of enemy
    if(type > 0) {
        specials_spawns[type-1] = current_enemies;
    }
}

void RoundManager::spawnEnemies(float dt, float pl_posX, float pl_posY, float pl_posZ, EntityMan& EM, bool isDefendingObjectiveActive, StressLevel& stress){
    
    increaseSpawnTime(dt);

    if(spawn_time >= TIME_SPAWN){
        if(spawn_enemy && current_enemies - dead_enemies < 30){
            EntityFactory* factory = EntityFactory::Instance();
            Waypoint_t* waypoint = sys_nav->getSpawnPoint(pl_posX, pl_posY, pl_posZ);
            Entity newEnemy{};

            if(specials_spawns[EXPLOSIVE_VECTOR_POS]!=-1 && specials_spawns[EXPLOSIVE_VECTOR_POS]>=current_enemies){
                newEnemy = factory->initEnemyExplosive(waypoint->x, waypoint->y, waypoint->z);
                specials_spawns[EXPLOSIVE_VECTOR_POS] = -1;
            }
            else if(specials_spawns[THROWER_VECTOR_POS]!=-1 && specials_spawns[THROWER_VECTOR_POS]>=current_enemies){
                newEnemy = factory->initEnemyThrower(waypoint->x, waypoint->y, waypoint->z);
                specials_spawns[THROWER_VECTOR_POS] = -1;
            }
            else if(specials_spawns[RUGBY_VECTOR_POS]!=-1 && specials_spawns[RUGBY_VECTOR_POS]>=current_enemies){
                newEnemy = factory->initEnemyRugby(waypoint->x, waypoint->y, waypoint->z);
                specials_spawns[RUGBY_VECTOR_POS] = -1;
            }
            else{
                uint8_t target_enemy_msk = PLAYER_TARGET_MASK;
                if(isDefendingObjectiveActive){
                    if(num_enemies_target_ply<=0 || (std::rand()%1==1 && num_enemies_target_obj>0)){
                        target_enemy_msk = OBJECTIVE_TARGET_MASK;
                        num_enemies_target_obj--;
                    }
                    else{
                        num_enemies_target_ply--;
                    }
                }

                newEnemy = factory->initEnemyNormal(waypoint->x, waypoint->y, waypoint->z, target_enemy_msk);
            }
            current_enemies++;

            // Modify max life depending on the round
            HealthComponent* healthCmp {&EM.getComponent<HealthComponent>(newEnemy)};
            healthCmp->health += healthCmp->health * (round_num/4 * 0.35);          // 35% more heterminadoalth every 4 rounds
        }
        
        spawn_time -= TIME_SPAWN;
    }

    if(current_enemies >= max_enemies && spawn_enemy){
        // Stop spawns
        spawn_enemy = false;
        pause_timer = TIME_BETWEEN_ROUNDS;
    }
    if(!spawn_enemy && dead_enemies >= max_enemies){
        // si es nueva horda pero el timer de la pausa es 0, la ronda ha terminado
        if(pause_timer==TIME_BETWEEN_ROUNDS && round_num!=0){
            using CList = MetaP::Typelist<>;
            using TList = MetaP::Typelist<PlayerTag>;
            std::vector<Entity*> player = EM.template search<CList, TList>();
            auto& playerSoundCmp = EM.getComponent<SoundComponent>(*player[0]);
            if(lastRound) {
                if(!lastRoundEnded) {
                    // Last round, congratulations message
                    pause_timer = 14.f;

                    // Play sound
                    playerSoundCmp.maskSounds[M_DIALOGUES] |= SOUND_PLAY;
                    playerSoundCmp.paramValues.insert_or_assign("radioDialogue", RADIO_DIALOGUE_PLAYING);
                    playerSoundCmp.paramValues.insert_or_assign("line", RADIO_COMPLEAT);

                    lastRoundEnded = true;
                }
            }
            else {
                // Round ended
                playerSoundCmp.maskSounds[M_DIALOGUES] |= SOUND_PLAY;
                playerSoundCmp.paramValues.insert_or_assign("playerDialogue", PLAYER_DIALOGUE_PLAYING);
                playerSoundCmp.paramValues.insert_or_assign("line", END_ROUND_SOUND);
            }
            
            stress.reset();
        }

        // nueva horda cuando el tiempo de pausa acabe
        if(pause_timer<=0){
            calculateMaxEnemiesRound(isDefendingObjectiveActive);

            // Reset variables for new round
            spawn_enemy     = true;
            spawn_time      = 0;
            dead_enemies    = 0;
            current_enemies = 0;
            pause_timer     = 0;

            using CList = MetaP::Typelist<>;
            using TList = MetaP::Typelist<PlayerTag>;
            std::vector<Entity*> player = EM.template search<CList, TList>();
            auto& playerSoundCmp = EM.getComponent<SoundComponent>(*player[0]);
            playerSoundCmp.maskSounds[M_NEW_ROUND] |= SOUND_PLAY;
        }
        else{
            pause_timer -= dt;
        }
    }
}

int RoundManager::getMaxEnemies(){
    return max_enemies;
}

void RoundManager::markLastRound() noexcept {
    lastRound = true;
}

void RoundManager::reset() {
    spawn_enemy             = false;
    lastRound               = false;
    lastRoundEnded          = false;
    spawn_time              = 0.0f;
    enemy_types_in_round    = 1;
    round_num               = 0;
    round_rest              = 0;
    max_enemies             = INITIAL_ENEMIES;
    dead_enemies            = INITIAL_ENEMIES;
    current_enemies         = 0;
    num_enemies_target_obj  = 0;
    num_enemies_target_ply  = 0;
    pause_timer             = 20.0f;
    specials_spawns         = {-1, -1, -1};
}