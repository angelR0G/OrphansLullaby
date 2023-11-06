#pragma once

const float MAX_PLAYER_INFECTION = 4.0f;

struct HealthComponent {
    float health{100.0f};
    float infection{0.0};
    uint8_t healthId{0};
    bool criticHealth{false};
};