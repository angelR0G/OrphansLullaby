#include "sceneEntity.hpp"
#include "sceneNode.hpp"

void GE::SceneEntity::draw(const glm::mat4& mat, ShaderResource* shader) {}

void GE::SceneEntity::setTreeNode(SceneNode* sn) {
    treeNode = sn;
}

GE::Transform* GE::SceneEntity::getTransform() {
    return &transform;
}

void GE::SceneEntity::setTranslation(glm::vec3 trans){
    transform.setTranslation(trans);
}

void GE::SceneEntity::setRotation(glm::vec3 rot){
    transform.setRotation(rot);
}

void GE::SceneEntity::setScale(glm::vec3 sc){
    transform.setScale(sc);
}

glm::vec3 GE::SceneEntity::getTranslation() const noexcept{
    return transform.getTranslation();
}

glm::vec3 GE::SceneEntity::getWorldLocation() const noexcept{
    glm::vec3 location      {transform.getTranslation()};
    SceneNode* parentNode   {treeNode->getParent()};

    // If has parent, add their translation
    if(parentNode != nullptr)
        location += parentNode->sEntity->getWorldLocation();

    return location;
}

glm::vec3 GE::SceneEntity::getWorldRotation() const noexcept{
    glm::vec3 rotation      {transform.getRotation()};
    SceneNode* parentNode   {treeNode->getParent()};

    // If has parent, add their rotation
    if(parentNode != nullptr)
        rotation += parentNode->sEntity->getWorldRotation();

    return rotation;
}

glm::vec3 GE::SceneEntity::getRotation() const noexcept{
    return transform.getRotation();
}

glm::vec3 GE::SceneEntity::getScale() const noexcept{
    return transform.getScale();
}

void GE::SceneEntity::setTransformMatrix(glm::mat4 transformMat){
    transform.setTransformMatrix(transformMat);
}

glm::mat4 GE::SceneEntity::getTransformMatrix(glm::mat4 matrix) noexcept{
    SceneNode* parentNode{treeNode->getParent()};

    // Check if has a parent entity
    if(parentNode != nullptr) {
        matrix = parentNode->sEntity->getTransformMatrix();
    }

    // Check if transform must be updated
    if(transform.getUpdate()) {
        // Transform is updating, so notify children
        treeNode->setUpdateChildren();
    }
    // Get transform matrix
    matrix = transform.getTransformMatrix(matrix);

    return matrix;
}

void GE::SceneEntity::setParent(SceneEntity* newParent) {
    SceneNode* newParentNode{nullptr};

    if(newParent != nullptr) 
        newParentNode = newParent->treeNode;

    treeNode->setParent(newParentNode);
}