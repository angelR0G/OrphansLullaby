#include "Profiler.hpp"

Profiler::~Profiler() {
    if(logFile.is_open()) {
        logFile.close();
    }        
}

Profiler::Profiler(const char* fileName, uint8_t numFields) {
    logFile.open(fileName, std::ios::out);
    profileObjects = numFields;
}

void Profiler::prepareProfiler() {
    profileTime = std::chrono::steady_clock::now();
}

void Profiler::saveProfilerData() {
    if(activateProfiler) {
        // Calculate time
        double time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-profileTime).count() / 1000.0f;

        // Increment and check objects
        ++objectsCount;
        if(objectsCount == profileObjects) {
            logFile << time << "\n";
            objectsCount = 0;
        }
        else {
            logFile << time << ",";
        }

        // Reset time
        profileTime = std::chrono::steady_clock::now();
    }
}

void Profiler::saveEmpty() {
    if(activateProfiler) {
        // Increment and check objects
        ++objectsCount;
        if(objectsCount == profileObjects) {
            logFile << -1 << "\n";
            objectsCount = 0;
        }
        else {
            logFile << -1 << ",";
        }
    }
}