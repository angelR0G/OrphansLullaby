#include "cinematicManager.hpp"

#include "graphic/engine.hpp"
#include "uiController.hpp"
#ifdef WINDOWS
    #include "../Fmod/SoundEngineWin.hpp"
#else
    #include "../Fmod/SoundEngine.hpp"
#endif
#include <chrono>

#define CINEMATIC_SKIP_TIME 1.5f


CinematicsManager::CinematicsManager() :
engine {GraphicEngine::Instance()}, ui{UiController::Instance()} {
    input.initializeInput();
}

void CinematicsManager::play(const std::string& videoPath, float fps, const std::string& soundPath) noexcept{
    using _clock = std::chrono::steady_clock;

    // Play cinematic and sound
    engine->playVideo(videoPath, fps);
    playCinematicSound(soundPath);

    // Create timer
    _clock::time_point oldTime = _clock::now();   

    bool videoPlaying {true};
    do {
        // Get elapsed time since last update
        auto now        {_clock::now()};
        float deltaTime {std::chrono::duration_cast<std::chrono::milliseconds>(now - oldTime).count() / 1000.0f};
        oldTime = now;
        
        // Update video
        engine->preRender();
        videoPlaying = engine->updateVideoFrame(deltaTime);
        if(skipTime > 0) ui->drawCinematicProgressBar(skipTime/CINEMATIC_SKIP_TIME);
        engine->postRender();

        // Check input for skiping cinemtic
        if(input.keyPress(SPACE))   skipTime += deltaTime;
        else                        skipTime -= deltaTime;

        if      (skipTime < 0)      skipTime = 0.f;
        else if (skipTime > CINEMATIC_SKIP_TIME) {
            // Skip button hold, skip cinematic
            engine->stopVideo();

            // Reset variables
            videoPlaying    = false;
            skipTime        = 0.f;
        }

    } while(videoPlaying);

    // Stop sound when cinematic ends
    if(sound != nullptr && !soundPath.empty()) sound->stopMenuSound(soundPath);
}

void CinematicsManager::initSoundEngine(SoundEngine* s) noexcept {
    sound = s;
}

void CinematicsManager::playCinematicSound(const std::string& path) noexcept {
    // Check sound engine has been initialized
    if(sound == nullptr) return;

    // Stop all sounds
    sound->stopAll();
    sound->update();

    // Play cinematic sound
    if(!path.empty()) sound->playMenuSound(true, path);
}