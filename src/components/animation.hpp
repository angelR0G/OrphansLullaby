#pragma once

#include <cstdint>
#include "../engine/graphic/resource/animations/bone.hpp"

//Forward declarations
struct AnimationResource;

//Enum for animation type
using animType = uint8_t;
enum class RunningAnimationEnemy : animType{
    WALK_ANIMATION       = 0,
    ATTACK_ANIMATION     = 1,
    ATTACK_ANIMATION1    = 2,
    DEATH_ANIMATION      = 3,
};
enum class RunningAnimationPlayer : animType{
    IDLE_ANIMATION      = 0,
    WALK_ANIMATION      = 1,
    SHOOT_ANIMATION     = 2,
    RELOAD_ANIMATION    = 3,
    DEATH_ANIMATION     = 4,
};

enum class RunningAnimationWeapon : animType{
    IDLE_ANIMATION      = 0,
    SHOOT_ANIMATION     = 1,
    RELOAD_ANIMATION    = 2,
};

enum class RunningAnimationTactShootgun : animType{
    IDLE_ANIMATION          = 0,
    SHOOT_ANIMATION         = 1,
    RELOAD_ANIMATION        = 2,
    FIRST_RELOAD_ANIMATION  = 3,
    LAST_RELOAD_ANIMATION   = 4,
};
enum class RunningAnimationRifle : animType{
    IDLE_ANIMATION          = 0,
    SHOOT_ANIMATION         = 1,
    RELOAD_ANIMATION        = 2,
    LAST_SHOOT_ANIMATION    = 3,
};

enum class RunningAnimationBarrel : animType{
    IDLE_ANIMATION          = 0,
    BREAK_ANIMATION         = 1,
    RESTORE_ANIMATION       = 2,
    IDLE_ANIMATION_BREAK    = 3,
};

struct Animation{
    bool loop{false};
    float maxDuration{};
    float framesPerSecond{};
    AnimationResource* animation{nullptr};
    std::array<glm::mat4, MAX_BONES> m_FinalBoneMatrices{};
    float m_CurrentTime{};
    int auxData{-1};
};

struct AnimationComponent{
    int rAnimation{static_cast<int>(RunningAnimationEnemy::WALK_ANIMATION)};
    int prevAnimation{static_cast<int>(RunningAnimationEnemy::WALK_ANIMATION)};
    bool needChange{false};
    float nextUpdate{0};
    std::vector<Animation> animations{};
};