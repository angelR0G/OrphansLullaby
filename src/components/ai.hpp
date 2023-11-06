#pragma once

#include "../utils/bt/behaviourtree.hpp"

enum class SB {
         Arrive
    ,    Seek
    ,    Flee
    ,    Pursue
    ,    AFK
};

struct AIComponent {
    double tx {0}, ty{0}, tz {0};
    bool tactive {false};

    float arrivalRadius {8.0f};
    
    SB behaviour { };

    //BehaviourTree* behaviourTree {nullptr};
    std::shared_ptr<BehaviourTree> behaviourTree{nullptr};

    float attackCD {};
    
    uint8_t targetMask {};

    // data structure = [x, y, z, destroy]
    //  - x, y, z are the point coordinates
    //  - destroy could be 1 or 0; 
    //     * 1 is robot has to stop movement for x seconds
    //     * 0 is keep moving when arrive
    std::vector<std::vector<float>> routeToFollow {};

    // at time x check if entity is visible for player to destroy it or not
    float time_alive {};
};