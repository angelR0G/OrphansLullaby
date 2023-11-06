#include "mesh.hpp"
#include "../engine/graphic/sceneTree/sceneMesh.hpp"
#include "../engine/graphic/engine.hpp"
#include "../engine/graphic/resource/resourceManager.hpp"

void MeshNode::createMeshNode(const char* model, const char* material){
    GraphicEngine* engine = GraphicEngine::Instance();
    mesh = engine->addSceneMesh();
    mesh->loadMesh(std::string(model));
    
    ResourceManager* resourceMan = ResourceManager::Instance();
    MaterialResource* mat = resourceMan->getMaterial(std::string(material));
    mesh->asignMaterial(mat);
}

void MeshNode::createAnimation(bool active, std::string animPath){
    ResourceManager* resourceMan = ResourceManager::Instance();
    mesh->asignAnimation(active, resourceMan->getAnimation(animPath, mesh->getMesh()));
}

MeshNode::~MeshNode() {}

void MeshNode::setPosition(float x, float y, float z) {
    mesh->setTranslation(glm::vec3(x, y, z));
}

void MeshNode::setScale(float sx, float sy, float sz) {
    mesh->setScale(glm::vec3(sx,sy,sz));
}

void MeshNode::setRotation(float rx, float ry, float rz){
    mesh->setRotation(glm::vec3(rx,ry,rz));
}

void MeshNode::setVisible(bool visible){
    mesh->setVisible(visible);
}

GE::SceneMesh* MeshNode::getMesh() {
    return mesh;
}

AnimationResource* MeshNode::getAnimation(){
    auto anims = mesh->getAnimation();
    return anims[anims.size()-1];
}

void MeshNode::setLODReferencePosition(float x, float y, float z) noexcept {
    GE::SceneMesh::setLODReferencePosition(x, y, z);
}

void MeshNode::setCastShadows(bool cast) noexcept {
    mesh->setCastShadows(cast);
}