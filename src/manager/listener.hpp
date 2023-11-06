#pragma once

struct Event;

struct Listener{
    void notify(Event*);
    void (*function)(Event*);
};