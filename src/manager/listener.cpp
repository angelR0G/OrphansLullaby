#include "listener.hpp"
#include "event.hpp"

void Listener::notify(Event* event){
    function(event);
}