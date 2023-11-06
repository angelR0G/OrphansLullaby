#pragma once
#include <cstdint>
#include <queue>
#include <memory>

#include "event.hpp"
#include "listener.hpp"

struct EventManager{
    struct ListenerRegistration{
        ListenerRegistration(Listener l, uint16_t c){
            listener        = l;
            listenerCode    = c;
        };
        
    
        uint16_t listenerId{nextId++};
        inline static uint16_t nextId{1};
        uint16_t listenerCode{};
        Listener listener;
    };
    static EventManager* Instance();

    ~EventManager();
    void scheduleEvent(std::shared_ptr<Event>);
    void registerListener(Listener, uint16_t);
    void dispatchEvents();
    void clearEvents();

private:
    EventManager();
    std::queue<std::shared_ptr<EventManager::ListenerRegistration>>   listenersQueue{};
    std::queue<std::shared_ptr<Event>>                                eventQueue{};
    inline static std::unique_ptr<EventManager>       pEventManager{nullptr};

};