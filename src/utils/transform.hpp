#pragma once

#include <vector>

struct Transform{
    Transform();
    Transform(std::vector<float>);
    std::vector<float> getPosition();
    std::vector<float> getRotation();
    std::vector<float> getScale();

    void setPosition(std::vector<float>);
    void setRotation(std::vector<float>);
    void setScale(std::vector<float>);

    float   x {}, y {} , z {},
            rx {}, ry {}, rz {},
            sx {}, sy {}, sz {};

};