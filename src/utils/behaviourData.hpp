#pragma once

#include "../engine/types.hpp"
#include <memory>
#include <vector>

struct NavigationComponent;
struct NavigationSystem;
struct AIComponent;

struct BehaviourData{
    using PtrFunction = void (NavigationSystem::*)(std::vector<float>, EntityMan*, size_t);

    BehaviourData(PtrFunction, int, int, std::vector<float>, EntityMan*, size_t, NavigationSystem*);
    void runFuntion();

    int phase, frec;
    PtrFunction function {nullptr};
    std::vector<float> floatparams;
    EntityMan* EM;
    size_t entId;
    NavigationSystem* nav_sys;
};