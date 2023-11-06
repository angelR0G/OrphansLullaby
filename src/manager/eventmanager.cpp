#include "eventmanager.hpp"

using ListenerReg = EventManager::ListenerRegistration;


EventManager* EventManager::Instance(){

    if(pEventManager.get() == nullptr){
        pEventManager  =  std::unique_ptr<EventManager>( new EventManager() );
    }

    return pEventManager.get();
}

EventManager::EventManager(){
    
}

EventManager::~EventManager(){
    //Empty the queues
    while(!eventQueue.empty()){
        Event* event = eventQueue.front().get();
        //delete event;
        eventQueue.pop();
    }
    while(!listenersQueue.empty()){
        ListenerReg* listReg = listenersQueue.front().get();
        //delete listReg;
        listenersQueue.pop();
    }
}

void EventManager::scheduleEvent(std::shared_ptr<Event> event){
    eventQueue.push(std::move(event));
}


void EventManager::registerListener(Listener l, uint16_t code){
    std::shared_ptr<ListenerReg> lr = std::shared_ptr<ListenerReg>(new ListenerReg(l, code));

    listenersQueue.push(std::move(lr));
    
}

void EventManager::dispatchEvents(){
    
    while (!eventQueue.empty())
    {
        Event* event = eventQueue.front().get();
        
        //Copy listeners queue to iterate throught the elements
        std::queue<std::shared_ptr<ListenerReg>> tempQueue = listenersQueue;
        while (tempQueue.size() > 0)
        {
            std::shared_ptr<ListenerReg> l = tempQueue.front();
            if(l.get()->listenerCode == event->eventCode){
                l.get()->listener.notify(event);
            }
            tempQueue.pop();

        }
        eventQueue.pop();
    }
    
}

void EventManager::clearEvents() {
    while(!listenersQueue.empty())
        listenersQueue.pop();
    
    while(!eventQueue.empty())
        eventQueue.pop();
}