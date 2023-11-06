#pragma once

#include "../engine/types.hpp"

struct SoundEngine;

struct SoundSystem {
    void update(EntityMan& EM);
    ~SoundSystem();
    void initialize(SoundEngine*);
    //void createSound(EntityMan&, Entity&);
private:
    SoundEngine* soundEngine {nullptr};
};