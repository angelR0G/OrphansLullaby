#include <stdexcept>
#include <iostream>

#include "manager/gamemanager.hpp"
#include "engine/menuController.hpp"
#include "engine/graphic/engine.hpp"
#ifdef WINDOWS
    #include "Fmod/SoundEngineWin.hpp"
#else
    #include "Fmod/SoundEngine.hpp"
#endif

#define GAME_STATE(i) state==i && gE->runEngine()
int main () {
    try {
        GraphicEngine* gE   = GraphicEngine::Instance();
        GameManager* gm     = GameManager::Instance();
        menuController* menu= menuController::Instance();
        //Create sound engine and load all basic banks from fmod
        std::unique_ptr<SoundEngine> soundEngine = std::unique_ptr<SoundEngine>( new SoundEngine(512, INCLUDE_EXTRA_DRIVER_DATA) );
        soundEngine->loadBasicBanks();
        //Give menu controller access to the soundEngine
        menu->setSoundEngine(soundEngine.get());

        // Intro cinematic
        menu->playCinematic(LOGO_CINEMATIC);

        int state{0};

        while(GAME_STATE(0)) {
            // Open menu
            soundEngine->playMenuSound(true, MENU_MUSIC);
            state = menu->run(START_MENU);
            
            while(GAME_STATE(1)) {
                // Init Game Manager
                gm->initGameManager(1, soundEngine.get());
                menu->playCinematic(INTRO_CINEMATIC);

                while(GAME_STATE(1)) {
                    // Start game
                    int newState = gm->game();
                    if(newState >= 1 && newState <= 3) {
                        // Open pause menu
                        if(newState == 1){
                            newState = menu->run(PAUSE_MENU);
                        // Defeat screen
                        }else if(newState == 2){
                            newState = menu->run(DEFEAT_MENU);
                        // Win screen
                        }else if(newState == 3){
                            menu->playCinematic(END_CINEMATIC);
                            newState = menu->run(WIN_MENU);
                        }
                        
                        if(newState >= 10)
                            state = 0;
                    }
                    else if(newState) {
                        state = 0;
                    }
                }
                gm->freeAllData();
            }
        }

    }catch (std::exception const& e){
        std::cerr << "EXCEPTION:";
        std::cerr << " " << e.what() << "\n"; 
    }
    
}