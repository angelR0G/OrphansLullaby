#pragma once
#include <vector>
#include <string>
#include <unordered_map>

#define SOUND_DEFAULT           0
#define SOUND_PLAY              1
#define SOUND_STOP              2
#define SOUND_PLAYING           4
#define SOUND_NOT_RELEASE       8
#define SOUND_3D                16
#define SOUND_UPDATE_PARAM      32
#define SOUND_UPDATE_POSITION   64
#define SOUND_DIALOGUE          128
#define SOUND_ALWAYS_UPDATE     256
#define SOUND_RELEASE           512


struct SoundComponent{
    std::vector<std::uint16_t> idEvent{};
    std::vector<std::uint16_t> idInstance{};
    std::vector<std::uint16_t>  maskSounds{};
    std::unordered_map<std::string, float> paramValues {};
};