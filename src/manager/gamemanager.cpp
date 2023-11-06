#include "gamemanager.hpp"
#include "eventmanager.hpp"

#include <cstdint>
#include <vector>
#include <unistd.h>
#include <chrono>

#include "engine/menuController.hpp"

#include "engine/entitymanager.hpp"
#include "engine/tools/memprinter.hpp"
#include "engine/tags.hpp"
#include "engine/physics/physicsEngine.hpp"
#include "engine/entityFactory.hpp"
#include "Fmod/soundFactory.hpp"
#include "IrrlichtFacade/image.hpp"

#include "utils/Profiler.hpp"
#include "utils/scheduler.hpp"
#include "utils/ObjectsCreator.hpp"
#include "utils/waypoint.hpp"
#include "utils/editorIA.hpp"
#include "utils/menuConfig.hpp"

#include "Fmod/MasksPositions.hpp"

#include "../engine/graphic/engine.hpp"
#include "../engine/graphic/resource/resourceManager.hpp"

#include "../engine/graphic/sceneTree/sceneNode.hpp"
#include "../engine/graphic/sceneTree/sceneCamera.hpp"
#include "../engine/graphic/sceneTree/sceneLight.hpp"
#include "../engine/graphic/sceneTree/sceneMesh.hpp"

//Points for each type of enemy
#define ENEMY_NORMAL_POINTS         10
#define ENEMY_EXPLOSIVE_POINTS      20
#define ENEMY_THROWER_POINTS        20
#define ENEMY_RUGBY_POINTS          50
#define HEADSHOT_POINTS_MULTIPLIER  1.5

// Profile macro
#define PROFILE(code) code profiler.saveProfilerData();
#define RESET_AND_PROFILE(code) profiler.prepareProfiler(); code profiler.saveProfilerData();
#define PROFILE_EMPTY profiler.saveEmpty();

//#define PROFILE(code) code
//#define RESET_AND_PROFILE(code) code
//#define PROFILE_EMPTY

const char* PROFILER_OUTPUT = "tools/profileLog.csv";
#define PROFILER_FIELDS 16

const float UPDATE_TICK_TIME    = 0.05f;
const float MAX_UPDATE_TIME     = 0.08f;

GameManager* GameManager::Instance(){

    if(pGameManager.get() == nullptr){
        pGameManager  = std::unique_ptr<GameManager>(new GameManager());
    }

    return pGameManager.get();
}

GameManager::GameManager(){
    std::srand(time(NULL));
}

GameManager::~GameManager(){}


int GameManager::game (){

    EventManager* eventMan = EventManager::Instance();
    Profiler profiler(PROFILER_OUTPUT, PROFILER_FIELDS);

    // Capture mouse for playing
    graphicEngine->captureMouse(true);

    float deltaTime{}, time{};
    std::chrono::steady_clock::time_point oldTime = std::chrono::steady_clock::now();    
    Scheduler* scheduler = Scheduler::Instance();
    gameState = 0;
    EditorIA editorIA = EditorIA();
    soundEngine->changePauseState("bus:/Generales", false);

    while(gameState == 0 && graphicEngine->runEngine()){
        auto now = std::chrono::steady_clock::now();
        deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - oldTime).count() / 1000.0f;
        time += deltaTime;
        oldTime = now;

        //Call scheduler
        scheduler->run();

        // Update input
        RESET_AND_PROFILE(gameState = sys_input.update(EM, deltaTime);)

        if(time >= UPDATE_TICK_TIME){
            // Check enemies spawn
            roundman.spawnEnemies(time, player_pos[0], player_pos[1], player_pos[2], EM, getEnemyTargetGoal(), stress);

            updateTimeStat(time);
            
            // Prevent errors from big update times
            time = std::min(time, MAX_UPDATE_TIME);
            
            // Update systems
            RESET_AND_PROFILE(sys_basic.update(EM, time, pStats);)
            PROFILE(sys_ai.             update(EM, *physicsEngine, time, getEnemyTargetGoal(), getCurrentGoal());)
            PROFILE(sys_mov.            update(EM, time, &player_pos);)
            PROFILE(sys_render.         update(EM, time, *graphicEngine);)
            PROFILE(sys_trigger.        update(EM, time);)
            PROFILE(sys_collision.      update(EM, time);)
            PROFILE(sys_health.         update(EM, time, pStats);)
            PROFILE(sys_nav.            update(EM, time, sys_ai.getIdTargets(EM));)
            PROFILE(sys_sound.          update(EM);)
            PROFILE(eventMan->dispatchEvents();)
            
            updateStress(time);
            updateZone();
            time -= UPDATE_TICK_TIME;
        }
        else {
            PROFILE_EMPTY
            PROFILE_EMPTY
            PROFILE_EMPTY
            PROFILE_EMPTY
            PROFILE_EMPTY
            PROFILE_EMPTY
            PROFILE_EMPTY
            PROFILE_EMPTY
            PROFILE_EMPTY
            PROFILE_EMPTY
        }

        // Update systems
        RESET_AND_PROFILE(sys_camera.update(EM, time/UPDATE_TICK_TIME);)
        PROFILE(sys_attack.update(EM, deltaTime);)
        PROFILE(sys_weapon.update(EM, *physicsEngine, deltaTime, wStats, stress);)

        // Delete entities
        EM.searchMarkedEntities(physicsEngine.get(), soundEngine, graphicEngine, deltaTime);
        
        // Render game
        graphicEngine->preRender();
        PROFILE(sys_animation.update(EM, deltaTime);)
        RESET_AND_PROFILE(sys_render.render(EM, *graphicEngine, time/UPDATE_TICK_TIME, deltaTime, rStats);)

        // Update editor ia
        editorIA.updateEditorIA(EM, roundman.getMaxEnemies(), &enemies_alive, ai_editor_active);

        // Debug methods
        if(gameState == 10) {
            updateDebugIndex();
            gameState = 0;
        }
        else debugUpdated = false;
        
        physicsEngine->debugPhysics(player_pos[0], player_pos[1], player_pos[2]);
        sys_nav.debugNavigation();
        graphicEngine->postRender();

        //On death
        if(pStats.health == 0){
            gameState = 2;
        }
    }

    // Pause sounds
    soundEngine->changePauseState("bus:/Generales", true);
    PROFILE(sys_sound.          update(EM);)

    // If game ended, sends statistics to menu controller
    if(gameState == 2 || gameState == 3) {
        gameStatistics.wave = rStats.wave-1;
        menuController::Instance()->updateLastGameStats(gameStatistics);
    }

    return gameState;
}
/*
void GameManager::initMenuScene() {
    // Get graphic engine
    graphicEngine = GraphicEngine::Instance();

    if(physicsEngine == nullptr)
        physicsEngine = std::unique_ptr<PhysicsEngine>(new PhysicsEngine(false) );

    // Load the necessary banks
    prepareBanks(2);

    // Initialize systems
    sys_sound.initialize(soundEngine.get());

    // Initialize managers and factories
    factory = EntityFactory::Instance();
    factory->initialize(EM, *physicsEngine, *graphicEngine);
    SoundFactory::Instance()->initializeEngine(soundEngine.get());

    createEntities(2);
}
*/
void GameManager::initGameManager(uint8_t mapIndex, SoundEngine* sE) {
    // Get graphic engine
    graphicEngine = GraphicEngine::Instance();

    //Get soundEngine
    soundEngine = sE;

    if(physicsEngine == nullptr)
        physicsEngine = std::unique_ptr<PhysicsEngine>(new PhysicsEngine(false) );

    // Load the necessary banks
    prepareBanks(mapIndex);

    // Init game manager variables
    points = 2000;
    pStats = playerStats{-1, -1};
    wStats = weaponStats{};
    rStats = roundStats{0, 0};
    gameFinished = false;
    stress.reset();

    // Initialize systems
    sys_sound.initialize(soundEngine);
    sys_input.initializeInput();
    sys_collision.initPhysics(physicsEngine.get());
    sys_trigger.initPhysics(physicsEngine.get());
    sys_nav.configureDebug(false, false, false);

    // Initialize managers and factories
    factory = EntityFactory::Instance();
    factory->initialize(EM, *physicsEngine, *graphicEngine);
    roundman.initializeRoundManager(&sys_nav);
    SoundFactory::Instance()->initializeEngine(soundEngine);

    // Load level information
    sys_nav.loadMapNavigation(mapIndex);
    sys_collision.loadMapCollisions(mapIndex);
    resetDoors();

    // Register events in event manager
    EventManager* eventMan = EventManager::Instance();
    Listener l = Listener{.function = &GameManager::increasePointsKill};
    eventMan->registerListener(l, EVENT_ENEMYDEAD);
    Listener lround = Listener{.function = &GameManager::increaseRound};
    eventMan->registerListener(lround, EVENT_ROUND);
    Listener lm1ke = Listener{.function = &GameManager::completeM1KEobjective};
    eventMan->registerListener(lm1ke, EVENT_M1KE_COMPLETE);
    Listener ldamage = Listener{.function = &GameManager::playerHit};
    eventMan->registerListener(ldamage, EVENT_PLAYER_DAMAGED);
    Listener lrespawn = Listener{.function = &GameManager::respawnEnemy};
    eventMan->registerListener(lrespawn, EVENT_RESPAWN_ENEMY);
    Listener lspawnEnemy = Listener{.function = &GameManager::spawnEnemy};
    eventMan->registerListener(lspawnEnemy, EVENT_SPAWN_NEW_ENEMY);

    createEntities(mapIndex);

    // Preload meshes to avoid doing it during game
    factory->preloadWeaponsAndEnemies();

    //Search player entity and assing to the enemies that entity
    using CList = MetaP::Typelist<>;
    using TList = MetaP::Typelist<PlayerTag>;
    std::vector<Entity*> player = EM.template search<CList, TList>();

    sys_ai.addNewTarget(player[0]->getId());

    // Initialize goals
    initGoals(mapIndex);

    // Reset game statistics
    gameStatistics.resetStats();

    updateConfigGame();
}

void GameManager::updateConfigGame(){
    menuController* mc = menuController::Instance();
    menuConfig config  = mc->getMenuConfig();

    mc->updateConfig();

    using CList = MetaP::Typelist<InputComponent>;
    using TList = MetaP::Typelist<PlayerTag>;
    EM.foreach<CList, TList>([&](Entity& e){
        InputComponent*   inputCmp = &EM.getComponent<InputComponent>(e);
        inputCmp->alternateRunning = (bool)config.sprintAlternate;
        inputCmp->mouseHorizontalSensitivity = 0.0085*(config.sensibilityX/100)+0.0001;
        inputCmp->mouseVerticalSensitivity = 0.0085*(config.sensibilityY/100)+0.0001;
    });
}

void GameManager::createEntities(uint8_t mapIndex){    
    // Generate map
    factory->initMap(mapIndex);

    // Generate map objects and get player spawn position
    Transform playerSpawn {generateMapObjects(mapIndex)};

    // Add all generated meshes to the render tree
    graphicEngine->updateSpatialTree();

    // Init player with their spawn position
    factory->initPlayer(playerSpawn);        
}

void GameManager::prepareBanks(uint8_t mapIndex){
    //Se cargan los bancos necesarios para el nivel en especifico
    if(mapIndex==1){
        soundEngine->loadBank("media/banks/AmbienteCentroComercial.bank");
        soundEngine->loadBank("media/banks/VocesRadio_Esp.bank");
        soundEngine->loadBank("media/banks/M1K3.bank");
    }
}

void GameManager::initGoals(uint8_t index) {
    if(index == 1) {
        goals = std::unique_ptr<Goals> ( new Goals(initMallGoals()) );

        // Create entities for the first map goals

        // Trigger to activate machine
        Entity* e = &factory->createTrigger(Transform{std::vector<float>{-1086, 164, -460, 0, 0, 0, 10, 10, 10}});
        goals->goalsId.emplace_back(e->getId());

        // Comms machine to defend
        e = &factory->createEnemyObjetive(Transform{std::vector<float>{-1086, 164, -460, 0, -90, 0, 10, 10, 10}});
        e->addTag<GoalDefendTag>();
        TriggerComponent* tCmp = &EM.getComponent<TriggerComponent>(*e);
        tCmp->bActive = false;
        goals->goalsId.emplace_back(e->getId());

        // Trigger to activate defend area
        e = &factory->createTrigger(Transform{std::vector<float>{-82, 203, -375, 0, 0, 0, 10, 10, 10}});
        RenderComponent* rcomp = &EM.addComponent<RenderComponent>(*e);
        rcomp->node = factory->createMesh(MODEL_GPS_ACTIVE_ZONE, TEXTURE_GPS_ACTIVE_ZONE);
        const float objectScale {SCALE_FACTOR_MAP * 0.75};
        rcomp->node.setScale(objectScale, objectScale, objectScale);
        rcomp->node.setVisible(false);
        goals->goalsId.emplace_back(e->getId());

        // Defend area limit
        e = &factory->createTrigger(Transform{{-82, 206, -405, 0, 0, 0, 80, 10, 80}});
        rcomp = &EM.addComponent<RenderComponent>(*e);
        rcomp->node = factory->createMesh(MODEL_DEFEND_AREA, TEXTURE_DEFEND_AREA);
        const float objectScale2 {SCALE_FACTOR_MAP * 1.6};
        rcomp->node.setScale(objectScale2, objectScale2, objectScale2);
        rcomp->node.setVisible(false);
        goals->goalsId.emplace_back(e->getId());

        // Gps
        e = &EM.createEntity(BasicComponent{.x=-82, .prevx=-82, .y=204, .prevy=204, .z=-375, .prevz=-375});
        rcomp = &EM.addComponent<RenderComponent>(*e);
        rcomp->node = factory->createMesh(MODEL_GPS, TEXTURE_GPS);
        rcomp->node.setScale(objectScale, objectScale, objectScale);
        rcomp->node.setVisible(false);
        goals->goalsId.emplace_back(e->getId());

        // Trigger to activate movable objective
        e = &factory->createTrigger(Transform{std::vector<float>{-40, 217, 640, 0, 0, 0, 10, 10, 10}});
        goals->goalsId.emplace_back(e->getId());

        // Movable Objective to protect
        e = &factory->createMovableEnemyObjetive(Transform{std::vector<float>{-40, 217, 640, 0, 180, 0, 10, 10, 10}});
        e->addTag<M1K3Tag>();
        tCmp = &EM.getComponent<TriggerComponent>(*e);
        tCmp->bActive = false;
        goals->goalsId.emplace_back(e->getId());

        // Obstruction destroyed by movable objective
        e = &factory->createObject( Transform{{-70, 214, -663, 0, 90, 0, 1.2, 1, 1.4}},
                                    Transform{{0, -3, -12, 0, 0, 0, 12, 7, 40}},
                                    index, 0);
        goals->goalsId.emplace_back(e->getId());

        // Electric panels hiden around all map (5 goal entities are created)
        createMallRandomElectricPanels();
        EventManager* eventMan = EventManager::Instance();
        Listener l = Listener{.function = &GameManager::breakElectricPanel};
        eventMan->registerListener(l, EVENT_BREAK_ELECTRIC_PANEL_GOAL);

        // Door blocked by electric panels
        e = &factory->createObject( Transform{{-82, 148, -905, 0, 0, 0, 1.835, 1.35, 1}},
                                    Transform{{0, 6, 0, 0, 0, 0, 38, 12, 6}},
                                    index, 1);
        goals->goalsId.emplace_back(e->getId());

        // Nest weak points to be destroyed (4 goal entities)
        createMallNest();

        nextGoal_mall(0);
    }
}

void GameManager::decreaseEnemy(){
    roundman.decreaseEnemy();
}

float GameManager::getFPS(){
    return fps;
}

std::uint16_t GameManager::getPoints(){
    return points;
}

playerStats GameManager::getPlayerStats(){
    return pStats;
}

weaponStats GameManager::getWeaponStats(){
    return wStats;
}

roundStats GameManager::getRoundStats(){
    return rStats;
}

void GameManager::playerHit(Event* event) {
    pGameManager->sys_render.getUiController()->addDamageMark(event->dataFloat, event->dataFloat2);

    pGameManager->stress.addStress(0.5f);
}

void GameManager::increasePoints(uint16_t p){
    points += p;

    
}

bool GameManager::checkEnoughtPoints(uint16_t p){
    if(points >= p) return true;
    return false;
}

//Increase the points checking the type of enemy killed
void GameManager::increasePointsKill(Event* event){
    unsigned int addPoints{};
    
    // Calculate points amount due to killed enemy type and update stats
    if(event->dataNum == ENEMY_NORMAL)     {
        addPoints += ENEMY_NORMAL_POINTS;
    }
    else {
        pGameManager->gameStatistics.specialKills++;
        if(     event->dataNum == ENEMY_EXPLOSIVE)  addPoints += ENEMY_EXPLOSIVE_POINTS;
        else if(event->dataNum == ENEMY_RUGBY)      addPoints += ENEMY_RUGBY_POINTS;
        else if(event->dataNum == ENEMY_THROWER)    addPoints += ENEMY_THROWER_POINTS;
    }
    pGameManager->gameStatistics.kills++;
    
    // Increase points if enemy died by head shot
    if(event->dataNum2 > 0) addPoints *= HEADSHOT_POINTS_MULTIPLIER;

    // Add points
    points  += addPoints;
    pGameManager->gameStatistics.points += addPoints;

    // Add stress
    pGameManager->stress.addStress(1.f);

    //Play sound 
    if(rand() % 100 < 10){
        pGameManager->activateSoundEvent(KILL_SOUND, "playerDialogue");
    }
}

//Increase round
void GameManager::increaseRound(Event* event){
    if(pGameManager->gameFinished) {
        // Finish game
        pGameManager->gameState = 3;
    }
    else {
        // Start new round
        ++rStats.wave;
        rStats.roundTime = ROUND_DISPLAY_TIME;
    }
}

void GameManager::respawnEnemy(Event* event){
    pGameManager->roundman.respawnEnemy(event->dataNum);
}

void GameManager::spawnEnemy(Event* event){
    pGameManager->roundman.spawnNewEnemy(event->dataNum, event->dataNum2);
}

void GameManager::deleteAllEntities(EntityMan& EM){
    using CList = MetaP::Typelist<BasicComponent>;
    using TList = MetaP::Typelist<>;
    // Mark all entities to destroy
    EM.foreach<CList, TList>([&](Entity& e){
        e.addTag<MarkToDestroyTag>();
    });

    // Destroy all entities
    EM.searchMarkedEntities(physicsEngine.get(), soundEngine, graphicEngine);    
}

void GameManager::resetDoors() {
    doors.clear();
}

void GameManager::addDoor(size_t entityID, std::pair<uint8_t, uint8_t> areas) {
    doors.insert({entityID, areas});
}

void GameManager::unlockDoor(size_t entityID) {
    using CList = MetaP::Typelist<>;
    using TList = MetaP::Typelist<PlayerTag>;
    auto doorI {doors.find(entityID)};
    std::vector<Entity*> player = EM.template search<CList, TList>();
    activateSoundEvent(NEW_ZONE_SOUND, "playerDialogue");
    auto& playerSoundCmp = EM.getComponent<SoundComponent>(*player[0]);
    playerSoundCmp.maskSounds[M_BUY] |= SOUND_PLAY;
    if(doorI != doors.end()) sys_nav.unlockDoorConnection(doorI->second);
}

void GameManager::updateStress(const float dt) {
    stress.addStress(-dt);
    using CList = MetaP::Typelist<>;
    using TList = MetaP::Typelist<PlayerTag>;
    std::vector<Entity*> playerEnt = EM.template search<CList, TList>();
    auto& playerSoundCmp = EM.getComponent<SoundComponent>(*playerEnt[0]);
    playerSoundCmp.maskSounds[M_ACTION_MUSIC] |= SOUND_UPDATE_PARAM;
    playerSoundCmp.paramValues.insert_or_assign("Combate", stress.getStress());
}

void GameManager::updateTimeStat(const float time) noexcept {
    // Add time
    gameStatistics.seconds += time;

    // Update minutes
    while(gameStatistics.seconds >= 60.f) {
        ++gameStatistics.minutes;
        gameStatistics.seconds -= 60.f;
    }
}

// Goal functions
Goals* GameManager::getGoals() {
    return goals.get();
}

bool GameManager::completeGoal() {
    bool completed {goals->completeGoal()};

    if(completed) {
        nextGoal_mall(goals->currentGoal);
    }

    return completed;
}

bool GameManager::completeGoal(float value) {
    bool completed {goals->completeGoal(value)};

    if(completed) {
        nextGoal_mall(goals->currentGoal);
    }

    return completed;
}

bool GameManager::completeGoal(int value) {
    bool completed {goals->completeGoal(value)};

    if(completed) {
        nextGoal_mall(goals->currentGoal);
    }

    return completed;
}

bool GameManager::getEnemyTargetGoal(){
    return goals->existsEnemyTarget();
}

void GameManager::allGoalsCompleted() noexcept {
    roundman.markLastRound();
    gameFinished = true;
}

void GameManager::nextGoal_mall(uint8_t index) {

    switch(index) {
        case 0:
            // Find comms machine
            activateInteractGoal(0);
            activateSoundEvent(RADIO_WELCOME, "radioDialogue");
            break;
        case 1:
            // Defend machine
            activateTrigger(goals->goalsId[1]);
            setNewAITarget(goals->goalsId[1]);
            activateSoundEvent(RADIO_STAY, "radioDialogue");
            roundman.spawnNewEnemy(10, 0);
            ++goal_count;
            break;
        case 2:
            // Place GPS on tree
            removeTargetAndDelete(goals->goalsId[1]);
            activateSoundEvent(RADIO_FIXMACHINE, "radioDialogue");
            activateInteractGoal(2);
            showGoal(2);
            ++goal_count;
            break;
        case 3:
            // Stay in tree area
            activateAreaGoal(3);
            activateSoundEvent(RADIO_CONNECTION, "radioDialogue");
            showGoal(3);
            showGoal(4);
            roundman.spawnNewEnemy(10, 0);
            ++goal_count;
            break;
        case 4:
            // Activate robot
            activateInteractGoal(5);
            activateGoalSound(M_M1K3_FLY, 6);
            activateSoundEvent(RADIO_SCORT, "radioDialogue");
            addDestroyableCollision(goals->goalsId[6], 4, 4, 4, 0, 3.0f, 0);
            showGoal(6);
            ++goal_count;
            break;
        case 5:
            // Escort robot
            setNewAITarget(goals->goalsId[6]);
            activateTrigger(goals->goalsId[6]);
            ++goal_count;
            break;
        case 6:
            // Destroy electric panels
            removeTargetAndDelete(goals->goalsId[6]);
            explodeGoal(goals->goalsId[6], ParticleEmitterType::ExplosionParticle);
            EM.getEntityById(goals->goalsId[7])->addTag<MarkToDestroyTag>();

            {
            float colX {SCALE_FACTOR_MAP*0.15};
            float colY {SCALE_FACTOR_MAP*0.25};
            float colZ {SCALE_FACTOR_MAP*0.15};
            addDestroyableCollision(goals->goalsId[8], colX, colY, colZ, 0, colY, 0);
            addDestroyableCollision(goals->goalsId[9], colX, colY, colZ, 0, colY, 0);
            addDestroyableCollision(goals->goalsId[10], colX, colY, colZ, 0, colY, 0);
            addDestroyableCollision(goals->goalsId[11], colX, colY, colZ, 0, colY, 0);
            addDestroyableCollision(goals->goalsId[12], colX, colY, colZ, 0, colY, 0);
            }
            activateGoalSound(M_ELECTRIC_PANEL, 8);
            activateGoalSound(M_ELECTRIC_PANEL, 9);
            activateGoalSound(M_ELECTRIC_PANEL, 10);
            activateGoalSound(M_ELECTRIC_PANEL, 11);
            activateGoalSound(M_ELECTRIC_PANEL, 12);

            activateSoundEvent(RADIO_LOCKED, "radioDialogue");
            ++goal_count;
            break;
        case 7:
            // Destroy nest
            EM.getEntityById(goals->goalsId[13])->addTag<MarkToDestroyTag>();
             activateSoundEvent(RADIO_OPEN, "radioDialogue");
            {
            float colX {SCALE_FACTOR_MAP*0.55};
            float colY {SCALE_FACTOR_MAP*0.55};
            float colZ {SCALE_FACTOR_MAP*0.55};
            addDestroyableCollision(goals->goalsId[14], colX, colY, colZ, 0, colY, 0);
            addDestroyableCollision(goals->goalsId[15], colX, colY, colZ, 0, colY, 0);
            addDestroyableCollision(goals->goalsId[16], colX, colY, colZ, 0, colY, 0);
            addDestroyableCollision(goals->goalsId[17], colX, colY, colZ, 0, colY, 0);
            }
            ++goal_count;
            break;
        default:
            allGoalsCompleted();
    }
}


void GameManager::activateSoundEvent(uint8_t soundID, string source){
    using CList = MetaP::Typelist<>;
    using TList = MetaP::Typelist<PlayerTag>;
    std::vector<Entity*> playerEnt = EM.template search<CList, TList>();
    auto& playerSoundCmp = EM.getComponent<SoundComponent>(*playerEnt[0]);
    playerSoundCmp.maskSounds[M_DIALOGUES] |= SOUND_PLAY;
    if(source.compare("radioDialogue")==0) playerSoundCmp.paramValues.insert_or_assign("radioDialogue", RADIO_DIALOGUE_PLAYING);
    else playerSoundCmp.paramValues.insert_or_assign("playerDialogue", PLAYER_DIALOGUE_PLAYING);
    playerSoundCmp.paramValues.insert_or_assign("line", soundID); 
}

void GameManager::activateGoalSound(uint8_t soundID, size_t i){
    Entity* e = EM.getEntityById(goals->goalsId[i]);

    if(e != nullptr && e->hasComponent<SoundComponent>()) {
        auto& soundCmp = EM.getComponent<SoundComponent>(*e);
        soundCmp.maskSounds[soundID] |= SOUND_PLAY;
    }
}

void GameManager::activateInteractGoal(size_t i) {
    Entity* e = EM.getEntityById(goals->goalsId[i]);
    if(e != nullptr) {
        e->addTag<GoalInteractTag>();
        // Enable trigger
        activateTrigger(e->getId());
    }
}

void GameManager::activateAreaGoal(size_t i) {
    Entity* e = EM.getEntityById(goals->goalsId[i]);
    if(e != nullptr) {
        e->addTag<GoalAreaTag>();

        // Enable trigger
        activateTrigger(e->getId());
    }
}

void GameManager::activateTrigger(size_t id) {
    Entity* e = EM.getEntityById(id);

    if(e != nullptr && e->hasComponent<TriggerComponent>()) {
        TriggerComponent* tCmp {&EM.getComponent<TriggerComponent>(*e)};

        tCmp->bActive = true;
    } 
}

void GameManager::setNewAITarget(size_t idGoal){
    sys_ai.addNewTarget(idGoal);
}

void GameManager::completeM1KEobjective(Event* ev){
    GameManager::Instance()->completeGoal();
}

void GameManager::createMallRandomElectricPanels() {
    const std::array<std::vector<float>, 5> electricPanels{
    //                      Position X, Y, Z                Rotation
    std::vector<float>{     -180.31f, 218.44f, 105.75f,     -90.f,
                            -756.71f, 217.81f, 324.21f,     90.f    },
    std::vector<float>{     -518.62f, 221.48f, 99.342f,     90.f,
                            -749.36f, 222.46f, -210.84f,    0.f     },
    std::vector<float>{     -119.49f, 163.02f, -827.15f,    0.f     },
    std::vector<float>{     242.25f, 221.47f, 119.78f,      -90.f,
                            269.28f, 222.21f, -110.00f,     180.f   },
    std::vector<float>{     -581.80f, 228.00f, -925.80f,    -90.f,
                            -799.12f, 218.79f, -799.12f,    0.f     }};
    
    // Create random electric panels
    for(auto& panelVector : electricPanels) {
        // Select a random panel
        size_t random {std::rand() % panelVector.size()/4};

        // Get electric panel transform
        Transform t{{   panelVector[4*random],  panelVector[4*random + 1],  panelVector[4*random + 2],  // Position
                        0,                      panelVector[4*random + 3]*3.1415f/180.f,  0,            // Rotation
                        0, 0, 0}};                                                                      // Scale

        // Create entity and save its ID
        Entity* panel = &factory->createElectricPanel(t);
        goals->goalsId.emplace_back(panel->getId());
    }
}

void GameManager::createMallNest() {
    // All nests transform
    std::vector<Transform> nestsTransform {
    //          Position                    Rotation
    Transform {{-24.0, 125.8, -1375.1,      90, 0, 0,   0, 0, 0}},
    Transform {{167.4, 141.8, -1283.7,      180, 0, 0,  0, 0, 0}},
    Transform {{83.0, 93.4, -1151.5,        0, -0, 0,   0, 0, 0}},
    Transform {{-129.5, 141.8, -1106.5,     180, 0, 0,  0, 0, 0}}
    };

    // Create each nest entity
    for(auto& nestTransform : nestsTransform) {
        // Create entity
        Entity* nest {&factory->createNestEntity(nestTransform)};

        // Add goal tag
        nest->addTag<MallNestTag>();

        // Add entity id to goalsId
        goals->goalsId.emplace_back(nest->getId());
    }
}

void GameManager::breakElectricPanel(Event* event) noexcept {
    auto& EM    {pGameManager->EM};
    Entity* e   {pGameManager->EM.getEntityById(event->dataNum)};

    // Check if an entity with given id exists
    if(e == nullptr) return;

    RenderComponent* render {&EM.getComponent<RenderComponent>(*e)};
    if(render->node.getMesh() != nullptr) {
        std::string path {"media/maps/map_props/"};
        pGameManager->graphicEngine->removeSceneMesh(render->node.getMesh());
        render->node.createMeshNode((path + "models/PanelElectricoRoto.obj").c_str(), (path + "materials/PanelElectricoRoto.mtl").c_str());
        render->node.setScale(SCALE_FACTOR_MAP, SCALE_FACTOR_MAP, SCALE_FACTOR_MAP);
    }

    // Delete its colision component
    if(e->hasComponent<CollisionComponent>()) {
        CollisionComponent* colision = &EM.getComponent<CollisionComponent>(*e);
        pGameManager->physicsEngine->deleteCollisionObject(&colision->collision);
        EM.removeComponent<CollisionComponent>(*e);
    }

    // Delete its health component
    if(e->hasComponent<HealthComponent>())
        EM.removeComponent<HealthComponent>(*e);
}

void GameManager::addDestroyableCollision(size_t idGoal, float sx, float sy, float sz, float offsetX, float offsetY, float offsetZ) {
    auto entity {EM.getEntityById(idGoal)};
    // Check if entity exists and already has component
    if(entity == nullptr || entity->hasComponent<CollisionComponent>()) return;

    // Add destroyable collision to entity
    CollisionComponent *colCmp = &EM.addComponent<CollisionComponent>(*entity);
    physicsEngine->addCollisionObject(colCmp, *entity, DESTROYABLE_COLLISION);
    physicsEngine->addBoxColliderToObject(&(colCmp->collision), offsetX, offsetY, offsetZ, sx, sy, sz);
}

void GameManager::removeTargetAndDelete(size_t idGoal) {
    // Remove target from AI system
    sys_ai.removeTarget(idGoal);

    // Search entity
    Entity* e = EM.getEntityById(idGoal);

    // Delete its AI component
    AIComponent* aiCmp = &EM.getComponent<AIComponent>(*e);
    //delete aiCmp->behaviourTree;
    aiCmp->behaviourTree = nullptr;
    e->removeComponent<AIComponent>();

    // Delete its Trigger component
    TriggerComponent* tCmp = &EM.getComponent<TriggerComponent>(*e);
    physicsEngine->deleteCollisionObject(&tCmp->collision);
    e->removeComponent<TriggerComponent>();
}

void GameManager::showGoal(size_t i) {
    Entity* e = EM.getEntityById(goals->goalsId[i]);

    if(e != nullptr && e->hasComponent<RenderComponent>()) {
        EM.getComponent<RenderComponent>(*e).node.setVisible(true);
    }
}

void GameManager::explodeGoal(size_t id, ParticleEmitterType type) noexcept {
    Entity* e = EM.getEntityById(id);

    if(e != nullptr && e->hasComponent<RenderComponent>()) {
        RenderComponent* render {&EM.getComponent<RenderComponent>(*e)};

        // Create explosion particles
        render->emitter.createParticleEmitter(type);
        render->emitter.setActiveForTime(0.2);

        // Destroy entity after a while
        e->addTag<MarkToDestroyDelayTag>();
    }
}

// change player global target depending on the boolean given by argument
// if is true: the global target will be the given by argument
// if is false: the global target will be the player
void GameManager::changePlayerTarget(bool newTarget, size_t id){
    if(newTarget){
        sys_ai.changeTarget(0, id);
    }
    else{
        using CList = MetaP::Typelist<>;
        using TList = MetaP::Typelist<PlayerTag>;
        std::vector<Entity*> player = EM.template search<CList, TList>();

        sys_ai.changeTarget(0, player[0]->getId());
    }
}

// Get current goal num
int GameManager::getCurrentGoal(){
    return goal_count;
}

//Add a new mesh from the map
void GameManager::addMapMesh(MeshNode meshNode){
    mapMeshes.push_back(meshNode);
}

//Update the reverb if we change from one zone to other
void GameManager:: updateZone(){
    float playerCluster = sys_nav.getPlayerCluster(player_pos[0], player_pos[1], player_pos[2]).reverb;
    if(soundEngine->getZone() != playerCluster){
        soundEngine->setZone(playerCluster);
    }
}


void GameManager::setTextCenterIMGUI(std::string text, std::vector<float> color, float offsetW, float offsetH, float fontSize, bool active){
    uiText.text = text;
    uiText.color = color;
    uiText.width = offsetW;
    uiText.height = offsetH;
    uiText.sizeText = fontSize;

    // Activate text
    if(active) uiText.active = active;
}

Text GameManager::getUiText(){
    return uiText;
}

void GameManager::activeText(bool active){
    uiText.active = active;
}

std::string GameManager::formatText(std::string text, std::vector<char> replaces, std::vector<std::string> substitutes){
    std::vector<std::string> seglist;

    if(replaces.size() != substitutes.size())
        assert("Error");

    for(size_t i{0}; i < replaces.size(); i++){
        std::string newText;
        int pos = text.find(replaces[i]);
        seglist.clear();
        seglist.push_back(text.substr(0, pos));
        seglist.push_back(substitutes[i]);
        seglist.push_back(text.substr(pos+1));
        for(size_t j{0}; j < seglist.size(); ++j){
            newText += seglist[j];
        }

        text = newText;
        
    }

    return text;
}

void GameManager::updateDebugIndex() noexcept {
    if(debugUpdated) return;

    // Update debug informatio to be shown
    ++debugIndex;
    if(debugIndex >= 6) debugIndex = 0;

    // Configure debug depending on the index
    if(     debugIndex == 0) physicsEngine->enableDebug(false);
    else if(debugIndex == 1) sys_nav.configureDebug(true, false, false);
    else if(debugIndex == 2) sys_nav.configureDebug(false, true, false);
    else if(debugIndex == 3) sys_nav.configureDebug(false, false, true);
    else if(debugIndex == 4) {sys_nav.configureDebug(false, false, false); physicsEngine->enableDebug(true);}
    else if(debugIndex == 5) ai_editor_active = true;

    if(debugIndex != 5 && ai_editor_active){
        ai_editor_active = false;
    }

    debugUpdated = true;
}

void GameManager::freeAllData() {
    deleteAllEntities(EM);

    graphicEngine->freeAllResources();
    
    sys_ai.clearTargets();
    sys_collision.clearCollisionData();
    sys_nav.clearMapNavigation();
    sys_render.reset();

    // Delete physics data AFTER all references have been deleted
    physicsEngine->freePhysicsData();

    goals = nullptr;

    roundman.reset();
    EventManager::Instance()->clearEvents();
    soundEngine->stopAll();
    soundEngine->release();
}