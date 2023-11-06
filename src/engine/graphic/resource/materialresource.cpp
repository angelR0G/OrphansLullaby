#include "materialresource.hpp"
#include "resourceManager.hpp"
#include <fstream>
#include <vector>
#include <string>

void MaterialResource::loadResource(std::string path){
    name = path;
    std::ifstream               readFile;
    readFile.open(path);
    std::string auxdirectory = path.substr(0, path.find_last_of('/'));
    std::string directory = auxdirectory.substr(0, auxdirectory.find_last_of('/')) + "/textures";
    try {
        // If finds the file, reads it
        if (!readFile.fail() && readFile.is_open()) {
            std::string line;

            // Initialize values
            size_t  nextPos;
            bool    nextData;
            std::vector<std::string>    data;

            while (readFile.good()) {
                // While there are lines to read, continues
                std::getline(readFile, line);

                // Split line for spaces
                data.clear();
                do {
                    nextPos     = line.find(' ');
                    nextData    = nextPos != line.npos;

                    if (nextData) {
                        data.emplace_back(line.substr(0, nextPos));
                        line = line.substr(nextPos+1);
                        data.emplace_back(line);
                    }
                } while(nextData);
                data.emplace_back(line);

                if(data.size() > 0) {
                    //Check material type
                    //Coeficient material data
                    if(data[0].compare("Ns") == 0){
                        textureCoeficient[ROUGHNESS_MAP] = stoi(data[1])/250.f;
                    }else if(data[0].compare("Ka") == 0){
                        textureCoeficient[METALLIC_MAP] = stoi(data[1].substr(0, data[1].find(' ')));
                    }else if(data[0].compare("Ke") == 0){
                        textureCoeficient[EMISSIVE_MAP] = stoi(data[1].substr(0, data[1].find(' ')));
                    }
                    //Map material data
                    else if(data[0].compare("map_Kd") == 0){
                        auto position = data[1].find_last_of('/');
                        if(position == std::string::npos)
                            texturePath[DIFFUSE_MAP] = directory + "/" + data[1];
                        else
                            texturePath[DIFFUSE_MAP] = directory + data[1].substr(data[1].find_last_of('/'));
                    }else if(data[0].compare("map_Ns") == 0){
                        auto position = data[1].find_last_of('/');
                        if(position == std::string::npos)
                            texturePath[ROUGHNESS_MAP] = directory + "/" + data[1];
                        else
                            texturePath[ROUGHNESS_MAP] = directory + data[1].substr(data[1].find_last_of('/'));
                    }else if(data[0].compare("map_refl") == 0){
                        auto position = data[1].find_last_of('/');
                        if(position == std::string::npos)
                            texturePath[METALLIC_MAP] = directory + "/" + data[1];
                        else
                            texturePath[METALLIC_MAP] = directory + data[1].substr(data[1].find_last_of('/'));
                    }else if(data[0].compare("map_Bump") == 0){
                        std::string auxPath = data[1].substr(data[1].find_last_of(' '));
                        auto position = auxPath.find_last_of('/');
                        if(position == std::string::npos)
                            texturePath[NORMAL_MAP] = directory + "/" + auxPath.substr(1);
                        else
                            texturePath[NORMAL_MAP] = directory + auxPath.substr(auxPath.find_last_of('/'));
                    }else if(data[0].compare("map_Ke") == 0){
                        auto position = data[1].find_last_of('/');
                        if(position == std::string::npos)
                            texturePath[EMISSIVE_MAP] = directory + "/" + data[1];
                        else
                            texturePath[EMISSIVE_MAP] = directory + data[1].substr(data[1].find_last_of('/'));
                    }
                } 
            }

            readFile.close();
            //Check if the material has a diffuse map, if not assing default texture
            if(texturePath[DIFFUSE_MAP].compare("") == 0){
                texturePath[DIFFUSE_MAP] = "media/maps/map2/textures/TexturaPared.png";
            }

            //Create textures for all maps
            textures[DIFFUSE_MAP] = ResourceManager::Instance()->getTexture(texturePath[DIFFUSE_MAP], std::string("tDiffuse"));
            if(texturePath[ROUGHNESS_MAP].compare("") != 0)      
                textures[ROUGHNESS_MAP]  = ResourceManager::Instance()->getTexture(texturePath[ROUGHNESS_MAP], std::string("tRoughness"));
            if(texturePath[METALLIC_MAP].compare("") != 0)  
                textures[METALLIC_MAP]   = ResourceManager::Instance()->getTexture(texturePath[METALLIC_MAP], std::string("tMetallic"));
            if(texturePath[NORMAL_MAP].compare("") != 0) 
                textures[NORMAL_MAP]   = ResourceManager::Instance()->getTexture(texturePath[NORMAL_MAP], std::string("tNormal"));
            if(texturePath[EMISSIVE_MAP].compare("") != 0) 
                textures[EMISSIVE_MAP]   = ResourceManager::Instance()->getTexture(texturePath[EMISSIVE_MAP], std::string("tEmissive"));
        }
    }catch (std::exception const& e){
        if(readFile.is_open())
            readFile.close();
    }
}

Texture MaterialResource::getTexture(int texture_type){
    //assert(texture_type > textures.size());
    if(textures[texture_type] != nullptr)
        return textures[texture_type]->getTexture();
    else 
        return Texture{.id = 0, .type="noTexture"};
}

float MaterialResource::getTextureCoeficient(size_t i) const noexcept{
    if(i < textureCoeficient.size())
        return textureCoeficient[i];
    else
        return 0.f;
}