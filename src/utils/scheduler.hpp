#pragma once

#include "behaviourData.hpp"
#include <queue>
#include <memory>

#define MAX_PATHFING_ENEMIES 3

struct Scheduler{
    static Scheduler* Instance();
    void addBehaviour(BehaviourData);
    void run();

    private:
        std::queue<BehaviourData> behaviours;
        int frame;
        inline static std::unique_ptr<Scheduler> pScheduler {nullptr};
};