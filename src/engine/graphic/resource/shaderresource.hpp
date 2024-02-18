#pragma once

#include "resource.hpp"

#include <glm/mat4x4.hpp>
#include <fstream>
#include <stdexcept>

struct ShaderResource : public Resource{
    
    ShaderResource();
    ~ShaderResource();

    unsigned int loadShader(std::string shaderFilePath, unsigned int type);
    unsigned int getShaderId();
    void setUniformMat4(int, glm::mat4 const&);
    void setUniformVec3(int, glm::vec3 const&);
    void setUniformVec4(int, glm::vec4 const&);
    void setUniformFloat(int, float const&);
    void setUniformInt(int, int const&);
    void setUniformVec2(int, glm::vec2 const&);
    private:
        int idShader;
};