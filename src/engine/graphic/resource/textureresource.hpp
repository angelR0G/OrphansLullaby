#pragma once

#include "resource.hpp"
#include "vertex.hpp"
#include "texture.hpp"

#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct TextureResource : public Resource{
    TextureResource();
    ~TextureResource();

    void loadResource(const std::string&, const std::string&);
    Texture getTexture();
    std::pair<int, int> getSize();
    static unsigned int loadCubeMap(std::vector<std::string>);
    bool updateTextureFromFile(const std::string&);

    private:
        unsigned int textureID;
        int width{0}, height{0};
        Texture texture;
        unsigned int TextureFromFile(const std::string&);
};