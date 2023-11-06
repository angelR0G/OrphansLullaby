#pragma once
#include <string>
#include <vector>

struct Text{
    bool active{false};
    std::string text{""};
    std::vector<float> color{1.0f, 1.0f, 1.0f, 1.0f};
    int width{0}, height{0};
    float sizeText{0};
};