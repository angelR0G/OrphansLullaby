#pragma once

struct GameStatistics {
    unsigned int    minutes {};
    float           seconds {};
    unsigned int    kills   {};
    unsigned int    specialKills {};
    unsigned int    wave    {};
    unsigned int    points  {};

    constexpr void resetStats() noexcept {
        minutes = kills = specialKills = wave = points = 0;
        seconds = 0.f;
    }
};