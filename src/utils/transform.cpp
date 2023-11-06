#include "transform.hpp"

Transform::Transform() {}

Transform::Transform(std::vector<float> initvalues){
    while(initvalues.size() < 9) {
        if(initvalues.size() >= 6)
            initvalues.push_back(1);
        else
            initvalues.push_back(0);
    }

    x  = initvalues[0];
    y  = initvalues[1];
    z  = initvalues[2];

    rx = initvalues[3];
    ry = initvalues[4];
    rz = initvalues[5];

    sx = initvalues[6];
    sy = initvalues[7];
    sz = initvalues[8];
}

std::vector<float> Transform::getPosition(){
    return std::vector<float> {x, y, z};
}

std::vector<float> Transform::getRotation(){
    return std::vector<float> {rx, ry, rz};
}

std::vector<float> Transform::getScale(){
    return std::vector<float> {sx, sy, sz};
}

void Transform::setPosition(std::vector<float> position){
    x = position[0];
    y = position[1];
    z = position[2];
}

void Transform::setRotation(std::vector<float> rotation){
    rx = rotation[0];
    ry = rotation[1];
    rz = rotation[2];
}

void Transform::setScale(std::vector<float> scale){
    sx = scale[0];
    sy = scale[1];
    sz = scale[2];
}