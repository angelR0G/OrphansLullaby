#pragma once

#include <cstdint>
#include <vector>
#include <variant>
#include <string>

enum class GoalTypes : uint8_t {
    NoGoal,
    ContinuousProgress,
    Count,
    Check
};

using goalsValue_t      = std::variant<float, int, bool>;
using progressData_t    = std::pair<goalsValue_t, goalsValue_t>;

struct GoalData {
    GoalData(GoalTypes, std::string const*, float,  bool eT = false, bool cT = false);
    GoalData(GoalTypes, std::string const*, bool,   bool eT = false, bool cT = false);
    GoalData(GoalTypes, std::string const*, int,    bool eT = false, bool cT = false);

    [[nodiscard]] GoalTypes     getType()           const noexcept;
    [[nodiscard]] std::string   getDescrption()     const noexcept;
    [[nodiscard]] bool          getEnemyTarget()    const noexcept;
    [[nodiscard]] bool          getCountTime()      const noexcept;
    [[nodiscard]] progressData_t getCurrentProgress() const noexcept;
    bool progress()         noexcept;
    bool progress(float)    noexcept;
    bool progress(int)      noexcept;

    private:
        GoalTypes       type {};
        std::string     description {""};
        goalsValue_t    objective {},
                        currentValue {};
        // Wheter there is a non-player target
        bool enemyTarget{false};
        // Whether the objetive is the time spent
        bool countTime{false};
};

struct Goals {
    inline virtual ~Goals() {
        goalsId.clear();
    }

    // Vector with entities ID
    std::vector<std::size_t> goalsId {};
    // Goals vector
    std::vector<GoalData> goals {};
    // Current goal
    uint8_t currentGoal {0};

    // Get current goal information
    [[nodiscard]] bool existsEnemyTarget()  const noexcept;
    [[nodiscard]] bool needToCountTime()    const noexcept;
    [[nodiscard]] GoalTypes         getType()               const noexcept;
    [[nodiscard]] std::string       getDescription()        const noexcept;
    [[nodiscard]] progressData_t    getCurrentProgress()    const noexcept;

    // Progress in the next goal and return whether is completed or not
    bool completeGoal();
    bool completeGoal(float);
    bool completeGoal(int);

    inline bool allGoalsComplete() {return currentGoal >= goals.size();}
};

[[nodiscard]] Goals initMallGoals() noexcept;