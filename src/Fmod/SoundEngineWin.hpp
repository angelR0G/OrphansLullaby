/*
FMOD Façade
*/

/*
Include section
*/

#pragma once

extern "C"{
    #include <windows/fmod/inc/fmod_studio.h>
    #include <windows/fmod/core/inc/fmod.h>
    #include <windows/fmod/core/inc/fmod_errors.h>
}

#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>
#include "../utils/transform.hpp"


#define INCLUDE_EXTRA_DRIVER_DATA 0
#define EXCLUDE_EXTRA_DRIVER_DATA 1

//Routes for VCA
#define VCA_EFFECTS_ROUTE         0
#define VCA_MUSIC_ROUTE           1
#define VCA_DIALOGUES_ROUTE       2
#define VCA_AMBIENT_ROUTE         3
#define VCA_MASTER_ROUTE          4

//Routes for non-ECS-dependant sounds
#define MENU_MUSIC                "event:/Música/Musica Menu Principal"
#define MENU_MOVE                 "event:/Menús/Desplazamiento"
#define MENU_ACCEPT               "event:/Menús/Aceptar"
#define MENU_REFUSE               "event:/Menús/Cancelar"

using namespace std;

//For playing dialogues
struct ProgrammerSoundContext{
    FMOD_SYSTEM* coreSys;
    FMOD_STUDIO_SYSTEM* sys;
    const char* dialogueString;
};


class SoundEngine{

    /*
    Public methods
    */
    public:
        /*
        Constructor and destructor
        */
        SoundEngine(int,int);
        ~SoundEngine();

        /*
        Engine management
        */
        void update();
        void release();

        /*
        Bank management
        */
        void loadBank(string);
        void unloadBank(string);
        void loadBasicBanks();

        /*
        Events and instance creation
        */
        void createDescriptor(string, uint16_t);
        uint16_t createInstance(uint16_t);
        uint16_t createInstance(uint16_t, Transform);
        uint16_t prepareSound(string, uint16_t, bool, Transform);
        void releaseSound(uint16_t);

        /*
        Events and instances management
        */
        void update3DInstance(Transform, uint16_t);
        int searchInstance(std::vector<uint16_t>, uint16_t);
        void playSound(uint16_t);
        void startSound(uint16_t);
        void stopSound(uint16_t);
        uint16_t prepareDialogue(string, uint16_t);
        void playDialogue(uint16_t, uint16_t, uint16_t);
        FMOD_STUDIO_EVENTDESCRIPTION* getDescriptor(uint16_t);
        void playMenuSound(bool, string);
        void stopMenuSound(string);

        /*
        Parameter management and listener update
        */
        void update3DListener(Transform);
        void setParameter(string, uint8_t, uint16_t, uint16_t);

        /*
        Volume control
        */
        float getVCAVolume(string);
        void setVCAVolume(string, float);

        /*
        Utilities
        */
        static bool getDialogueState();
        void enhanceDialogues();
        void changePauseState(string, bool);
        void stopAll();
        void setZone(float);
        float getZone();
        void setPauseState(uint8_t);

        /*
        Callbacks
        */
        static FMOD_RESULT F_CALLBACK programmerSoundCallback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE* event, void *parameters);

    private:
        /*
        Sound engine essentials
        */
        void *extraDriverData;
        FMOD_STUDIO_SYSTEM* system;
        FMOD_SYSTEM* coreSystem;

        /*
        Storage
        */
        std::unordered_map<string, FMOD_STUDIO_BANK*> banks;
        std::unordered_map<uint16_t, FMOD_STUDIO_EVENTDESCRIPTION*> eventDescriptions;
        std::vector<FMOD_STUDIO_EVENTINSTANCE*> eventInstances;

        /*
        Vector for the dialogue selection:
            - 0 for player 1 for radio
            - 0-19 new Round 20-27 kill, 28 no points, 29-32 new zone 33-35 buy weapon 36-38 no ammo
        */
        inline static std::vector<std::vector<const char*>> dialogueVector{{"endRound1","endRound2","endRound3","endRound4","endRound5","endRound6","endRound7","endRound8","endRound9","endRound10","endRound11", "endRound12", "endRound13", "endRound14", "endRound15", "endRound16", "endRound17", "endRound18", "endRound19", "endRound20","kill1", "kill2", "kill3", "kill4", "kill5", "kill6", "kill7", "kill8", "noPoints", "newZone1", "newZone2", "newZone3", "newZone4", "buyWeapon1", "buyWeapon2", "buyWeapon3", "noAmmo1", "noAmmo2", "noAmmo3"},
        {"welcome", "machineFixed", "issue", "connection", "scort", "locked", "open", "defend", "compleat", "stay"}};

        /*
        Dialogue state, vca routes and player location for ambient and reverb
        */
        inline static bool isDialoguePlaying {false}, isDialogueEnhanced{false};
        inline static std::vector<string> vcaRoutes {"vca:/VCA-Efectos", "vca:/VCA-Musica", "vca:/VCA-Dialogos", "vca:/VCA-Ambiente", "vca:/VCA-Master"};
        inline static float zone{1};

        /*
        Dialogue parameters
        */
        FMOD_STUDIO_PARAMETER_DESCRIPTION paramDesc;
        FMOD_STUDIO_PARAMETER_ID parameterID;
        ProgrammerSoundContext programmerSoundContext;
};



