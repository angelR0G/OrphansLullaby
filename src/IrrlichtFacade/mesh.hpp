#pragma once

#include <string>
#include <stdexcept>
#include <cstdint>

//Forward declarations
namespace GE{
    struct SceneMesh;
};
struct AnimationResource;

#define LIGHTING      0


struct MeshNode {

    //MeshNode(const char*, const char*);
    MeshNode() = default;
    void createMeshNode(const char*, const char*);
    void createAnimation(bool, std::string);
    ~MeshNode();
    void setPosition(float x, float y, float z);
    void setScale(float sx, float sy, float sz);
    void setRotation(float rx, float ry, float rz);
    void setVisible(bool);
    GE::SceneMesh* getMesh();
    AnimationResource* getAnimation();
    void setCastShadows(bool) noexcept;
    
    static void setLODReferencePosition(float x, float y, float z) noexcept;
    private:
        GE::SceneMesh* mesh{nullptr};
};