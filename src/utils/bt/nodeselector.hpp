#pragma once


struct BTNodeSelector : BTNode {
    using value_type     = BTNode*;
    using container_type = std::vector<value_type>;
    using init_list      = std::initializer_list<value_type>;
    using iterator       = std::vector<value_type>::iterator;

    BTNodeSelector(init_list l) : nodes{l} {};

    void reset() { currentNodeIt = nodes.begin(); }
    
	BTNodeStatus run(EntityContext* ectx) noexcept final{
        if( currentNodeIt == nodes.end() ){
            reset();
        }

        auto status = (*currentNodeIt)->run(ectx);

        switch(status){
            case BTNodeStatus::running: { 
                return BTNodeStatus::running;
            }

            case BTNodeStatus::success: {
                reset();
                return BTNodeStatus::success; 
            }

            case BTNodeStatus::fail:    {
                ++currentNodeIt;
                if( currentNodeIt == nodes.end() ){
                    reset();
                    return BTNodeStatus::fail;
                }
            }
            
            return BTNodeStatus::running;
        }
    }

    private:
        container_type nodes {};
        iterator       currentNodeIt {nodes.begin()};

};