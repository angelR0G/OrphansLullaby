#pragma once

#include <chrono>
#include <fstream>

class Profiler {
    bool                                    activateProfiler {true};
    std::chrono::steady_clock::time_point   profileTime {std::chrono::steady_clock::now()};
    std::ofstream                           logFile;
    uint8_t                                 profileObjects{10};
    uint8_t                                 objectsCount{0};
    
    public:
        Profiler(const char*, uint8_t);
        ~Profiler();

        void prepareProfiler();
        void saveProfilerData();
        void saveEmpty();
};