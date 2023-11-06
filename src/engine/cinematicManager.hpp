#pragma once
#include <memory>
#include <string>

#include "../systems/input.hpp"

struct GraphicEngine;
struct UiController;
struct SoundEngine;

struct CinematicsManager{

    public:
        CinematicsManager();
        ~CinematicsManager() = default;

        void play(const std::string&, float, const std::string&) noexcept;
        void initSoundEngine(SoundEngine*) noexcept;

    private:
        GraphicEngine* engine   {nullptr};
        SoundEngine* sound      {nullptr};
        InputSystem input       {};
        UiController* ui        {nullptr};
        float skipTime          {};

        void playCinematicSound(const std::string&) noexcept;
};