#include "textureresource.hpp"

#include <stb/stb_image.h>
#include <glad/glad.h> 

TextureResource::TextureResource(){}
TextureResource::~TextureResource(){
    glDeleteTextures(1, &textureID);
}

unsigned int TextureResource::TextureFromFile(const std::string& path){

    glGenTextures(1, &textureID);

    int nrComponents;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format{GL_RGB};
        /*if (nrComponents == 1)
            format = GL_RED;
        else */if (nrComponents <= 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;


        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    }
    
    stbi_image_free(data);

    return textureID;
}

bool TextureResource::updateTextureFromFile(const std::string& path){
    bool updated {false};
    int nrComponents;

    // Try to load data from path
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format {GL_RGB};
        if (nrComponents <= 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        // Load image texture
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        updated = true;
    }

    // Free data
    stbi_image_free(data);

    return updated;
}

unsigned int TextureResource::loadCubeMap(std::vector<std::string> skyboxFaces){

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < skyboxFaces.size(); i++)
    {
        unsigned char *data = stbi_load(skyboxFaces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void TextureResource::loadResource(const std::string& path, const std::string& typeName){
    name = path;
    texture.id = TextureFromFile(path);
    texture.type = typeName;
}

Texture TextureResource::getTexture(){
    return texture;
}

//Return the size from a texture
//A pair with width, height
std::pair<int, int> TextureResource::getSize() {
    return std::pair<int, int>(width, height);
}