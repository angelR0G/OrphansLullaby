#pragma once

struct Waypoint_t;

#define NAVIGATION_UPDATE_TIME  5

#define NEED_TO_UPDATE_FALSE    0
#define NEED_TO_UPDATE_TRUE     1
#define NEED_TO_UPDATE_RUNNING  2

#define pathfinding_weights_type std::array<int8_t, 1>
//  Weights:
//      Visibility
#define MAX_WEIGHT_VALUE 127

struct NavigationComponent {
    std::vector<Waypoint_t*> path {};
    pathfinding_weights_type pathWeights{};

    uint8_t needToUpdate    {NEED_TO_UPDATE_FALSE };
    float   updateTime      {NAVIGATION_UPDATE_TIME};
};