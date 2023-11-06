#pragma once

#include <map>

#include "roundmanager.hpp"

#include "systems/basic.hpp"
#include "systems/render.hpp"
#include "systems/input.hpp"
#include "systems/camera.hpp"
#include "systems/movement.hpp"
#include "systems/sound.hpp"
#include "systems/ai.hpp"
#include "systems/collision.hpp"
#include "systems/trigger.hpp"
#include "systems/weapon.hpp"
#include "systems/navigation.hpp"
#include "systems/health.hpp"
#include "systems/attack.hpp"
#include "systems/animation.hpp"

#include "utils/editorIA.hpp"

#include "playerStats.hpp"
#include "weaponStats.hpp"
#include "../IrrlichtFacade/text.hpp"
#include "roundStats.hpp"
#include "goals.hpp"
#include "../utils/stressLevel.hpp"
#include "../utils/gameStatistics.hpp"

#define ROUND_DISPLAY_TIME 4.5

struct EntityFactory;
struct UiController;
struct Event;

struct GameManager{
    static GameManager* Instance();
    ~GameManager();
    GameManager(const GameManager&) = delete;
    GameManager(GameManager&)       = delete;
    void initGameManager(uint8_t, SoundEngine*);
    void initMenuScene();

    static void increasePointsKill(Event*);
    static void increasePoints(uint16_t);
    static bool checkEnoughtPoints(uint16_t);
    static void increaseRound(Event*);
    static void completeM1KEobjective(Event *);
    static void playerHit(Event*);
    static void respawnEnemy(Event*);
    static void spawnEnemy(Event*);

    // temporal for uagames demo
    void addPoints();
    void tpToCoord(float, float);
    void addRounds();

    int game();
    void decreaseEnemy();
    float getFPS();
    void resetDoors();
    void addDoor(size_t, std::pair<uint8_t, uint8_t>);
    void unlockDoor(size_t);

    void updateConfigGame();

    static std::uint16_t getPoints();
    static playerStats getPlayerStats();
    static weaponStats getWeaponStats();
    void setTextCenterIMGUI(std::string text, std::vector<float> color, float offsetW, float offsetH, float fontSize, bool active = true);
    Text getUiText();
    void activeText(bool);
    std::string formatText(std::string, std::vector<char> replaces, std::vector<std::string> substitutes);
    static roundStats  getRoundStats();

    Goals* getGoals();
    void allGoalsCompleted() noexcept;
    bool completeGoal();
    bool completeGoal(float);
    bool completeGoal(int);
    int  getCurrentGoal();
    bool getEnemyTargetGoal();
    void prepareBanks(uint8_t);
    void changePlayerTarget(bool, size_t);
    void explodeGoal(size_t, ParticleEmitterType) noexcept;

    void addMapMesh(MeshNode);
    void updateZone();

    void updateDebugIndex() noexcept;
    void freeAllData();

    // Entity manager
    EntityMan       EM;

    StressLevel stress{};
private:
    GameManager();

    void deleteAllEntities(EntityMan&);
    void createEntities(uint8_t);
    void initGoals(uint8_t);
    
    GraphicEngine*                  graphicEngine{nullptr};
    EntityFactory*                  factory {nullptr};
    std::unique_ptr<PhysicsEngine>  physicsEngine {nullptr};
    SoundEngine*                    soundEngine {nullptr};
    uint8_t         gameState{0};
    bool            gameFinished{false};

    // Round manager pointer
    RoundManager roundman;

    // Fps
    float fps{0.0f};
    uint8_t debugIndex {0};
    bool debugUpdated {false};
    GameStatistics gameStatistics{};

    // Map doors
    std::map<size_t, std::pair<uint8_t, uint8_t>> doors {};

    // Player position
    std::array<float, 3> player_pos{0, 0, 0};

    // HUD variables
    inline static std::uint16_t points {};
    inline static playerStats   pStats {};
    inline static weaponStats   wStats {};
    Text uiText;
    inline static roundStats    rStats {1,ROUND_DISPLAY_TIME};

    // AI Editor variables
    std::vector<IAParameters> enemies_alive {};
    bool ai_editor_active {false};

    // Game Manager pointer
    inline static std::unique_ptr<GameManager> pGameManager{nullptr};

    // Game systems
    BasicSystem         sys_basic;
    InputSystem         sys_input;
    CameraSystem        sys_camera;
    RenderSystem        sys_render;
    MovementSystem      sys_mov;
    AISystem            sys_ai;
    WeaponSystem        sys_weapon;
    CollisionSystem     sys_collision;
    TriggerSystem       sys_trigger;
    SoundSystem         sys_sound;
    NavigationSystem    sys_nav;
    HealthSystem        sys_health;
    AttackSystem        sys_attack;
    AnimationSystem     sys_animation;

    //Map meshes
    std::vector<MeshNode> mapMeshes{};

    // Goals struct
    std::unique_ptr<Goals> goals{nullptr};

    // Current goal num
    size_t goal_count {0};

    // Mall goals functions
    void nextGoal_mall(uint8_t);
    
    void activateSoundEvent(uint8_t, std::string);
    void activateInteractGoal(size_t);
    void activateGoalSound(uint8_t, size_t);
    void activateAreaGoal(size_t);
    void showGoal(size_t);
    void activateTrigger(size_t);
    void setNewAITarget(size_t);
    void removeTargetAndDelete(size_t);
    void addDestroyableCollision(size_t, float, float, float, float offsetX = 0.f, float offsetY = 0.f, float offsetZ = 0.f);
    static void breakElectricPanel(Event*) noexcept;
    void createMallRandomElectricPanels();
    void createMallNest();
    void updateStress(const float);
    void updateTimeStat(const float) noexcept;
};