#include "programresource.hpp"
#include <stdexcept>

#include <glad/glad.h>

ProgramResource::ProgramResource() :
    programID(0)
{
    //create the program object
    programID = glCreateProgram();
    if(programID == 0)
        throw std::runtime_error("glCreateProgram failed");
}

void ProgramResource::initProgram(unsigned int idVertexShader, unsigned int idFragmentShader){
    
    //attach the shader
    glAttachShader(programID, idVertexShader);
    glAttachShader(programID, idFragmentShader);

    //link the shaders together
    glLinkProgram(programID);

    //detach the shader
    glDeleteShader(idVertexShader);
    glDeleteShader(idFragmentShader);

    //throw exception if linking failed
    GLint status;
    glGetProgramiv(programID, GL_LINK_STATUS, &status);
    
    if (status == GL_FALSE) {
        std::string msg("Program linking failure: ");

        GLint infoLogLength;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* strInfoLog = new char[infoLogLength + 1];
        glGetProgramInfoLog(programID, infoLogLength, NULL, strInfoLog);
        msg += strInfoLog;
        delete[] strInfoLog;

        glDeleteProgram(programID); programID = 0;
        throw std::runtime_error(msg);
    }
}

void ProgramResource::initProgram(std::vector<unsigned int> idShaders){
    
    //attach the shader
    for(auto idShader : idShaders){
        glAttachShader(programID, idShader);
    }

    //link the shaders together
    glLinkProgram(programID);

    //detach the shader
    for(auto idShader : idShaders){
        glDeleteShader(idShader);
    }

    //throw exception if linking failed
    GLint status;
    glGetProgramiv(programID, GL_LINK_STATUS, &status);
    
    if (status == GL_FALSE) {
        std::string msg("Program linking failure: ");

        GLint infoLogLength;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* strInfoLog = new char[infoLogLength + 1];
        glGetProgramInfoLog(programID, infoLogLength, NULL, strInfoLog);
        msg += strInfoLog;
        delete[] strInfoLog;

        glDeleteProgram(programID); programID = 0;
        throw std::runtime_error(msg);
    }
}

ProgramResource::~ProgramResource()
{
    if(programID != 0) glDeleteProgram(programID);
}

//this function return the programID
unsigned int ProgramResource::getProgramId() const
{
    return programID;
}

//this function return the handle of arrribute variable
unsigned int ProgramResource::createAttribute(const char* attribName) const{

    GLint attrib = glGetAttribLocation(programID, attribName);

    return attrib;
}
//this function return the handle of uniform variable
unsigned int ProgramResource::createUniform(const char* uniformName) const{

    GLint uniform = glGetUniformLocation(programID, uniformName);

    return uniform;
}