/*
Include section
*/
#pragma once
#include "cstdint"
#ifdef WINDOWS
    #include "../Fmod/SoundEngineWin.hpp"
#else
    #include "../Fmod/SoundEngine.hpp"
#endif
#include <memory>

class SoundFactory{

    public:
        static SoundFactory* Instance();
        ~SoundFactory();
        void initializeEngine(SoundEngine*);
        uint16_t createInstanceFromEvent(uint16_t, bool, Transform);
        uint16_t createDialogue(uint16_t);
        
    private:
        SoundFactory();
        SoundEngine* soundEngine = nullptr;
        inline static std::unique_ptr<SoundFactory> pSoundFactory{nullptr};
        
};