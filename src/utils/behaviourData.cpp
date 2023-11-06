#include "behaviourData.hpp"
#include "../systems/navigation.hpp"

BehaviourData::BehaviourData(PtrFunction f, int fr, int p, std::vector<float> fp, EntityMan* em, size_t id, NavigationSystem* nsys){
    function    = f;
    frec        = fr;
    phase       = p;
    floatparams = fp;

    entId       = id;
    EM          = em;
    nav_sys     = nsys;
}

void BehaviourData::runFuntion(){
    (nav_sys->*function)(floatparams, EM, entId);
}