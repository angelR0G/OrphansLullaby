#include "image.hpp"
#include <IMGUI/imgui.h>

#include "../engine/graphic/resource/textureresource.hpp"
#include "../engine/graphic/resource/resourceManager.hpp"

ImVec2 addImVec2(const ImVec2& base, const ImVec2& add) {
    return ImVec2(base.x + add.x, base.y + add.y);
}

ImVec2 ImRotate(const ImVec2& v, float cos_a, float sin_a) {
    return ImVec2(v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a);
}

ImageNode::ImageNode(std::string texturePath, float pX, float pY, float h, float w) : posx(pX), posy(pY){
    
    //Get resource manager and get the texture with the path
    ResourceManager* resMan = ResourceManager::Instance();
    texture = resMan->getTexture(texturePath);

    //Assign the size for the texture
    std::pair<int, int> size = texture->getSize();
    if(h <= 0)  height = size.second; 
    else        height = h;

    if(w <= 0)  width = size.first;
    else        width = w;
}

ImageNode::~ImageNode(){}


void ImageNode::drawImage(){
    //Set the image position
    ImGui::SetCursorPos(ImVec2(posx, posy));
    
    //Draw image
    ImGui::Image((void*)(intptr_t) texture->getTexture().id, ImVec2(width, height), ImVec2(0,0), ImVec2(1,1), ImVec4(color[0], color[1], color[2], color[3]));
}

void ImageNode::drawRotatedImage(const float& angle) {
    // Get draw list
    ImDrawList* drawList {ImGui::GetWindowDrawList()};

    // Calculate angle's cosine and sine
    float c {cosf(angle)};
    float s {sinf(angle)};

    // Calculate vertices position
    ImVec2 origin   {posx, posy};
    float x         {width * 0.5f};
    float y         {height * 0.5f};

    ImVec2 v0 {addImVec2(origin, ImRotate({-x, -y}, c, s))};
    ImVec2 v1 {addImVec2(origin, ImRotate({ x, -y}, c, s))};
    ImVec2 v2 {addImVec2(origin, ImRotate({ x,  y}, c, s))};
    ImVec2 v3 {addImVec2(origin, ImRotate({-x,  y}, c, s))};

    // Draw image
    drawList->AddImageQuad((void*)(intptr_t) texture->getTexture().id, 
                            v0, v1, v2, v3, 
                            ImVec2(0, 0), ImVec2(1, 0), ImVec2(1, 1), ImVec2(0, 1), 
                            IM_COL32(color[0]*255, color[1]*255, color[2]*255, color[3]*255));
}

//Return the texture size(width, height)
std::pair<float, float> ImageNode::getTextureSize(){
    return texture->getSize();
}

//Return the image size(width, height)
std::pair<unsigned int, unsigned int> ImageNode::getSize(){
    return std::pair<unsigned int, unsigned int> {width, height};
}

//Set the image size at the screen (width, height)
void ImageNode::setSize(std::pair<float, float> size){
    width   = size.first;
    height  = size.second;
}

//Return the image position (x, y)
std::pair<float, float> ImageNode::getPosition(){
    return std::pair<float, float> {posx, posy};
}

//Set the image position at the screen (x, y)
void ImageNode::setPosition(std::pair<float, float> position){
    posx = position.first;
    posy = position.second;
}

std::array<float, 4> ImageNode::getColor(){
    return color;
}
void ImageNode::setColor(std::array<float, 4> c){
    color = c;
}