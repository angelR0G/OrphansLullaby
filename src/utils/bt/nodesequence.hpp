#pragma once

#include "node.hpp"
#include <vector>
#include <initializer_list>
#include "../../engine/physics/physicsEngine.hpp"

struct BTNodeSequence : BTNode {
    using value_type     = BTNode*;
    using container_type = std::vector<value_type>;
    using init_list      = std::initializer_list<value_type>;
    using iterator       = std::vector<value_type>::iterator;

    BTNodeSequence(init_list l) : nodes{l} {};

    void reset() { currentNodeIt = nodes.begin(); }
    
	BTNodeStatus run(EntityContext* ectx) noexcept final {
        if( currentNodeIt == nodes.end() ){
            reset();
        }
        
        auto status = (*currentNodeIt)->run(ectx);

        switch(status){
            case BTNodeStatus::running: { 
                return BTNodeStatus::running; 
            }
            case BTNodeStatus::fail:    {
                reset();
                return BTNodeStatus::fail;
            }
            case BTNodeStatus::success: {
                ++currentNodeIt;
                if( currentNodeIt == nodes.end() ){
                    reset();
                    return BTNodeStatus::success; 
                }

            }

            return BTNodeStatus::running;
        }
    }

    private:
        container_type nodes {};
        iterator       currentNodeIt {nodes.begin()};

};