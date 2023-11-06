#pragma once
#include <vector>
#include "resource.hpp"

struct ProgramResource : public Resource{
    public:
        ProgramResource();
        ~ProgramResource();
        ProgramResource(const ProgramResource&)     = delete;
        ProgramResource(ProgramResource&)           = delete;

        void initProgram(unsigned int, unsigned int);
        void initProgram(std::vector<unsigned int>);
        unsigned int getProgramId() const;
        unsigned int createAttribute(const char* attribName) const;
        unsigned int createUniform(const char* uniformName) const;

    private:
        unsigned int programID;

};