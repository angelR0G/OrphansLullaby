#include "shaderresource.hpp"

#include <glad/glad.h>


ShaderResource::ShaderResource(){
    idShader = 0;
}

ShaderResource::~ShaderResource(){}


unsigned int ShaderResource::loadShader(std::string shaderFilePath, unsigned int type) {
    // Open file with the given path
    std::ifstream shaderFile;
    shaderFile.open(shaderFilePath);

    try {
        // Check if the file is open
        if(!shaderFile.fail() && shaderFile.is_open()) {
            std::string src {""};
            std::string line{""};

            // Saves file in a string
            while(shaderFile.good()) {
                std::getline(shaderFile, line);
                src += line + "\n";
            }

            // Close file
            shaderFile.close();

            // Get the shader ID
            idShader = glCreateShader(type);

            // Create a shader with the read code
            const char* source = src.c_str();
            glShaderSource(idShader, 1, &source, NULL);
            glCompileShader(idShader);

            GLint status;
            glGetShaderiv(idShader, GL_COMPILE_STATUS, &status);
            if (status == GL_FALSE)
            {
                std::string msg("Compile failure in shader:\n");

                GLint infoLogLength;
                glGetShaderiv(idShader, GL_INFO_LOG_LENGTH, &infoLogLength);
                char* strInfoLog = new char[infoLogLength + 1];
                glGetShaderInfoLog(idShader, infoLogLength, NULL, strInfoLog);
                msg += strInfoLog;
                delete[] strInfoLog;

                glDeleteShader(idShader); idShader = 0;
                throw std::runtime_error(msg);
            }
        }
    } catch(std::exception const& e) {
        if(shaderFile.is_open())
            shaderFile.close();
    }

    return idShader;
}

unsigned int ShaderResource::getShaderId(){
    return idShader;
}

void ShaderResource::setUniformMat4(int uniform, glm::mat4 const& mat){
    glUniformMatrix4fv(uniform, 1, GL_FALSE, &mat[0][0]);
}

void ShaderResource::setUniformVec3(int uniform, glm::vec3 const& vec){
    glUniform3fv(uniform, 1, &vec[0]);
}

void ShaderResource::setUniformVec4(int uniform, glm::vec4 const& vec){
    glUniform4fv(uniform, 1, &vec[0]);
}

void ShaderResource::setUniformFloat(int uniform, float const& f){
    glUniform1f(uniform, f);
}

void ShaderResource::setUniformInt(int uniform, int const& i){
    glUniform1i(uniform, i);
}