#pragma once

#include "resource.hpp"
#include "textureresource.hpp"
#include <string>
#include <array>

#define NUMBER_MAPS     5
#define DIFFUSE_MAP     0
#define ROUGHNESS_MAP   1
#define METALLIC_MAP    2
#define NORMAL_MAP      3
#define EMISSIVE_MAP    4

struct MaterialResource : public Resource{

    void loadResource(std::string);
    Texture getTexture(int);

    [[nodiscard]] float getTextureCoeficient(size_t i) const noexcept;

    [[nodiscard]] constexpr size_t getTexturesNumber() const noexcept {
        return NUMBER_MAPS;
    }
    private:
        //Array to store all different maps for texutres
        //Diffuse, roughness, metallic, normal
        std::array<std::string, NUMBER_MAPS> texturePath{"", "", "", "", ""};
        //If no path for texture, store a coeficient 
        std::array<float, NUMBER_MAPS> textureCoeficient{0.f, 0.f, 0.f, 0.f, 0.f};
        std::array<TextureResource*, NUMBER_MAPS> textures;
};