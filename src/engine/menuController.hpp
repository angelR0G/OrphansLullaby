#pragma once

#include "menu.hpp"
#include "menuaction.hpp"
#include "../systems/input.hpp"
#include "../systems/render.hpp"
#include "../utils/menuConfig.hpp"
#include "../utils/gameStatistics.hpp"
#include "engine/cinematicManager.hpp"
#ifdef WINDOWS
    #include "../Fmod/SoundEngineWin.hpp"
#else
    #include "../Fmod/SoundEngine.hpp"
#endif

#include <memory>
#include <cstdint>
#include <vector>

#include <fstream>

// Cinematics index
#define LOGO_CINEMATIC      0
#define CREDITS_CINEMATIC   1
#define INTRO_CINEMATIC     2
#define END_CINEMATIC       3

struct GraphicEngine;

struct menuController{

    using actionOption = std::pair<int, int>;

    public:
        static menuController* Instance();
        ~menuController();

        void initialize();
        int update(int action, int option, bool ingame);

        void readOptionsDocument();
        void writeOptionsDocument();
        void valueValidation();

        int updateOption(int action, int option, bool ingame);
        void updateMenuConfigOption(float nvalue);
        void updateMenuConfigOption(float nvalue, int optSel);

        int getNumberOptions();
        int getCurrentOption();
        int getOptionHovered();
        int getCurrentMenuID();
        std::vector<const char*> getCurrentNames();

        void changeToMenu(menu menuc);
        void changeToMenu(int menuid);
        void alternateOptionsMenu(int move);
        int sidesOptionsMenu(int move);

        int getType(int num);
        float getValue(int num);
        float getMaxValue(int num);
        float getMinValue(int num);
        float* getValuePointer(int num);
        float getGapValue(int num);
        menuConfig getMenuConfig();

        const char* getFirstName(int menuid);

        void setIngame(bool);
        void setOptionSelected(int o_p);
        void setOptionHovered(int o_p);
        void setValue(int o_p, float nvalue);
        void setSoundEngine(SoundEngine*);
        void menuSounds(actionOption, int);
        void hoverSound(int);
        void setSideHover(int);
        void clickSound();

        void updateConfig();

        int run(int);

        void playCinematic(size_t) noexcept;

        void updateLastGameStats(const GameStatistics&) noexcept;
        GameStatistics getLastGameStats() noexcept;

    private:
        menuController();
        inline static std::unique_ptr<menuController> pmenuController{nullptr};
        std::vector<menu> menus;

        GraphicEngine*  graphicEngine{nullptr};
        InputSystem     sys_input;
        RenderSystem    sys_render;
        SoundEngine*    soundEngine{nullptr};
        CinematicsManager cineManager;

        // Keys control
        bool spaceValid     {false};
        bool escapeValid    {false};

        //
        UiController* uiController{nullptr};
    
        menu currentMenu;
        int id;
        int numberOptions;
        int optionSelected, optionHovered, sideHover;

        bool ingame{false};
        int  mainMenu{START_MENU};

        int stateChange{0};

        menuConfig mc;
        std::fstream optionsFile;
        GameStatistics lastGameStats {};
};