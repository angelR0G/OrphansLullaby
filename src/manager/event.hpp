#pragma once
#include <cstdint>
#include "eventCodes.hpp"

struct Event{
    Event(uint16_t type){
        eventCode = type;
    }
    Event(uint16_t type, int dN) {
        eventCode   = type;
        dataNum     = dN;
    }
    uint16_t    eventId{nextId++};
    uint16_t    eventCode{EVENT_DEFAULT};
    int         dataNum;
    int         dataNum2;
    float       dataFloat;
    float       dataFloat2;
    
    inline static uint16_t nextId{1};
};