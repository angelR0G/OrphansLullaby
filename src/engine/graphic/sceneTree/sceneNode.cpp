#include "sceneNode.hpp"
#include "../engine.hpp"

GE::SceneNode* GE::SceneNode::addChild(std::unique_ptr<SceneNode> newChild){
    // Set child's parent
    newChild.get()->parent = this;

    // Add new child to children vector
    children.push_back(std::move(newChild));

    return children.back().get();
}

void GE::SceneNode::removeChild(SceneNode* removeChild){
    for(size_t i{}; i < children.size(); ++i){
        if(children[i].get() == removeChild){
            children.erase(children.begin()+i);
        }
    }
}

GE::SceneNode* GE::SceneNode::getParent(){
    return parent;
}

void GE::SceneNode::run(bool update, mat4 tMatrixAcum, ShaderResource* shader, std::vector<FrustrumPlane> cameraFrustrum, bool useFrustrum){
    if(sEntity != nullptr) {
        Transform* transform = sEntity->getTransform();

        if(update) {
            // Notify if a previous node has been updated
            transform->setUpdate();
        }
        if(transform->getUpdate()){
            // Notify children to update
            update = true;
            tMatrixAcum = transform->updateTransformMatrix(tMatrixAcum);
        }

        // Get transform matrix
        //tMatrixAcum = transform->getTransformMatrix(tMatrixAcum);
    
        // Draw entity
        //sEntity->draw(tMatrixAcum, shader, cameraFrustrum, useFrustrum);
    }

    // Draw all its children
    for(size_t i{}; i < children.size(); ++i){
        children[i]->run(update, tMatrixAcum, shader, cameraFrustrum, useFrustrum);
    }
}

void GE::SceneNode::deleteNode(bool deleteChildren) {

    if(!deleteChildren) {
        // Move children to the parent
        for(size_t i{0}; i<children.size(); ++i) {
            parent->addChild(std::move(children[i]));
        }
    }
    children.clear();

    // Delete this node from the parent
    if(parent != nullptr)
        parent->removeChild(this);
}

void GE::SceneNode::setUpdateChildren() {
    size_t size{children.size()};

    for(size_t i=0; i<size; ++i) {
        children[i]->sEntity->getTransform()->setUpdate();
    }
}

void GE::SceneNode::setParent(SceneNode* newParent) {
    if(newParent == nullptr)
        newParent = GraphicEngine::Instance()->sceneRoot.get();

    // Remove current parent node
    std::unique_ptr<SceneNode> temporalReference{nullptr};
    auto& parentChildren {parent->children};
    for(size_t i{0}; i < parentChildren.size(); ++i) {
        if(parentChildren[i].get() == this) {
            temporalReference = std::move(parentChildren[i]);
            parentChildren.erase(parentChildren.begin() + i);
        }
    }

    // Set new parent
    newParent->addChild(std::move(temporalReference));
}