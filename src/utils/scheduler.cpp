#include "scheduler.hpp"

Scheduler* Scheduler::Instance(){
    if(pScheduler.get()==nullptr){
        pScheduler = std::unique_ptr<Scheduler>(new Scheduler());
    }
    
    return pScheduler.get();
}

void Scheduler::addBehaviour(BehaviourData data){
    behaviours.push(data);
}

void Scheduler::run(){
    if(frame < 60)
        frame++;
    else frame = 0;

    bool maxEntitiesProcess = false; 
    int aux = 1;
    while(behaviours.size() > 0 && !maxEntitiesProcess){
        BehaviourData data = behaviours.front();
        if(data.frec % (frame+data.phase)){
            data.runFuntion();
            behaviours.pop();
        }

        if(aux>MAX_PATHFING_ENEMIES){
            maxEntitiesProcess = true;
        }

        ++aux;
    }
}