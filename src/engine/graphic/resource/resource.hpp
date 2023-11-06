#pragma once
#include <string>

struct Resource{

    virtual ~Resource() = default;
    
    std::string getName();
    
    protected:
        std::string name;
};