#pragma once

#include <string>
#include <array>

//Texture Resource
struct TextureResource;

struct ImageNode {
    ImageNode(std::string, float pX = 0.f, float pY = 0.f, float h = -1.f, float w = -1.f);
    ~ImageNode();

    void drawImage();
    void drawRotatedImage(const float&);
    std::pair<float, float> getTextureSize();
    std::pair<unsigned int, unsigned int> getSize();
    void setSize(std::pair<float, float>);
    std::pair<float, float> getPosition();
    void setPosition(std::pair<float, float>);
    std::array<float, 4> getColor();
    void setColor(std::array<float, 4>);
    
    private:
        TextureResource* texture{nullptr};
        float height, width;
        float posx{}, posy{};
        std::array<float, 4> color{1.f, 1.f, 1.f, 1.f};
};