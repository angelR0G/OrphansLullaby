#include "goals.hpp"
#include <assert.h>

#define DEFAULT_GOAL_TYPE           GoalTypes::Check
#define DEFAULT_GOAL_DESCRIPTION    "Acaba con todos los grull."

#define M1GOAL_DEFEND_MACHINE_TIME  20.f
#define M1GOAL_DEFEND_AREA_TIME     50.f

/***************************
 * GOAL DATA
***************************/
GoalData::GoalData(GoalTypes t, std::string const* d, float o, bool eT, bool cT) :
type{t}, description{*d}, objective{o}, enemyTarget{eT}, countTime{cT} {
    // Check goals type are consistent with the specified type
    if(type != GoalTypes::ContinuousProgress) {
        static_assert("Goal does not match float type.");
    }
    currentValue = 0.f;
}

GoalData::GoalData(GoalTypes t, std::string const* d, bool o, bool eT, bool cT) :
type{t}, description{*d}, objective{o}, enemyTarget{eT}, countTime{cT} {
    // Check goals type are consistent with the specified type
    if(type != GoalTypes::Check) {
        static_assert("Goal does not match bool type.");
    }
    currentValue = false;
}

GoalData::GoalData(GoalTypes t, std::string const* d, int o, bool eT, bool cT) :
type{t}, description{*d}, objective{o}, enemyTarget{eT}, countTime{cT} {
    // Check goals type are consistent with the specified type
    if(type != GoalTypes::Count) {
        static_assert("Goal does not match int type.");
    }
    currentValue = 0;
}

GoalTypes   GoalData::getType()         const noexcept {return type;}
std::string GoalData::getDescrption()   const noexcept {return description;}
bool        GoalData::getEnemyTarget()  const noexcept {return enemyTarget;}
bool        GoalData::getCountTime()    const noexcept {return countTime;}

progressData_t GoalData::getCurrentProgress() const noexcept {
    return {currentValue, objective};
}

bool GoalData::progress() noexcept {
    if(type == GoalTypes::Check) {
        currentValue = true;
        return true;
    }

    return false;
}

bool GoalData::progress(float v) noexcept {
    if(type == GoalTypes::ContinuousProgress) {
        std::get<float>(currentValue) += v;
        if(currentValue >= objective) return true;
    }

    return false;
}

bool GoalData::progress(int v) noexcept {
    if(type == GoalTypes::Count) {
        std::get<int>(currentValue) += v;
        if(currentValue >= objective) return true;
    }

    return false;
}

/***************************
 * GOALS
***************************/
bool Goals::existsEnemyTarget() const noexcept {
    // Check if goal index is correct
    if(currentGoal >= goals.size()) return false;

    return goals[currentGoal].getEnemyTarget();
}

bool Goals::needToCountTime() const noexcept {
    // Check if goal index is correct
    if(currentGoal >= goals.size()) return false;

    return goals[currentGoal].getCountTime();
}

GoalTypes Goals::getType() const noexcept {
    // Check if goal index is correct
    if(currentGoal >= goals.size()) return DEFAULT_GOAL_TYPE;

    return goals[currentGoal].getType();
}

std::string Goals::getDescription() const noexcept {
    // Check if goal index is correct
    if(currentGoal >= goals.size()) return DEFAULT_GOAL_DESCRIPTION;

    return goals[currentGoal].getDescrption();
}

progressData_t Goals::getCurrentProgress() const noexcept {
    // Check if goal index is correct
    if(currentGoal >= goals.size()) return {};

    return goals[currentGoal].getCurrentProgress();
}

bool Goals::completeGoal() {
    // Check if goal index is correct
    if(currentGoal >= goals.size()) return false;

    // Update goal
    bool completed{goals[currentGoal].progress()};

    // If goal is completed, increase goals index
    if(completed) currentGoal++;

    return completed;
}

bool Goals::completeGoal(float v) {
    // Check if goal index is correct
    if(currentGoal >= goals.size()) return false;

    // Update goal
    bool completed{goals[currentGoal].progress(v)};

    // If goal is completed, increase goals index
    if(completed) currentGoal++;

    return completed;
}

bool Goals::completeGoal(int v) {
    // Check if goal index is correct
    if(currentGoal >= goals.size()) return false;

    // Update goal
    bool completed{goals[currentGoal].progress(v)};

    // If goal is completed, increase goals index
    if(completed) currentGoal++;

    return completed;
}

/***************************
 * MAP 1 GOALS
***************************/
Goals initMallGoals() noexcept {
    Goals goals {};
    std::string description {};

    // First goal: find broken comms machine
    description = "Encuentra una máquina de comunicaciones dañada en el interior del centro comercial.";
    goals.goals.emplace_back(GoalTypes::Check, 
                            &description,
                            true);

    // Second goal: defend comms machine
    description = "Defiende la máquina de comunicaciones.";
    goals.goals.emplace_back(GoalTypes::ContinuousProgress,
                            &description,
                            M1GOAL_DEFEND_MACHINE_TIME,
                            true, true);

    // Third goal: activate christmas tree defense
    description = "Coloca el dispositivo de geolocalización en el árbol de navidad central.";
    goals.goals.emplace_back(GoalTypes::Check, 
                            &description,
                            true);

    // Fourth goal: defend christmas tree
    description = "Mantente cerca del árbol de navidad.";
    goals.goals.emplace_back(GoalTypes::ContinuousProgress,
                            &description,
                            M1GOAL_DEFEND_AREA_TIME);

    // Fifth goal: activate the robot
    description = "Activa a M1K3 en el aparcamiento.";
    goals.goals.emplace_back(GoalTypes::Check,
                            &description,
                            true);

    // Sixt goal: escort the robot
    description = "Escolta a M1K3 hasta su objetivo.";
    goals.goals.emplace_back(GoalTypes::Check,
                            &description,
                            true, 
                            true);

    // Seventh goal: find and destroy electric panels
    description = "Encuentra y destruye los paneles eléctricos que bloquean la puerta.";
    goals.goals.emplace_back(GoalTypes::Count,
                            &description,
                            5);

    // Last goal: destroy the nest
    description = "Destruye el nido de los grull.";
    goals.goals.emplace_back(GoalTypes::Count,
                            &description,
                            4);

    return goals;
}