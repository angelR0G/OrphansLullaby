#include "transform.hpp" 
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

GE::Transform::Transform() : transformMatrix(1){}

void GE::Transform::setTranslation(vec3 trans) noexcept{
    update = true;
    translation = trans;
}

void GE::Transform::translate(vec3 trans) noexcept {
    update = true;
    translation += trans;
}

void GE::Transform::setRotation(vec3 rot) noexcept{
    update = true;
    rotation = rot;
}

void GE::Transform::setScale(vec3 sc) noexcept{
    update = true;
    scale = sc;
}

void GE::Transform::setTransformMatrix(mat4 transform) noexcept{
    transformMatrix = transform;
}

bool isIdentity(const glm::mat4& mat) {
    return (mat[0][0] > 0.9999 && mat[0][1] < 0.0001 && mat[0][2] < 0.0001 && mat[0][3] < 0.0001 &&
            mat[1][0] < 0.0001 && mat[1][1] > 0.9999 && mat[1][2] < 0.0001 && mat[1][3] < 0.0001 &&
            mat[2][0] < 0.0001 && mat[2][1] < 0.0001 && mat[2][2] > 0.9999 && mat[2][3] < 0.0001 &&
            mat[3][0] < 0.0001 && mat[3][1] < 0.0001 && mat[3][2] < 0.0001 && mat[3][3] > 0.9999);
}

glm::mat4 GE::Transform::updateTransformMatrix(mat4 accumulateTransform){
    // Generate a identity matrix
    mat4 transform(1.0f);

    // Apply translation
    transform = glm::translate(transform, translation);

    // Apply rotation
    transform = transform * getRotationMatrixFromRotation();

    // Apply scale
    transform = glm::scale(transform, vec3(-scale.x, scale.y, scale.z));

    // Apply accumulated transform matrix
    if(isIdentity(accumulateTransform)) transformMatrix = transform;
    else                                transformMatrix = accumulateTransform * transform;
    
    update = false;

    return transformMatrix;

}

glm::mat4 GE::Transform::getRotationMatrixFromRotation() const noexcept {
    quat quaternionRot(rotation);
    return glm::toMat4(quaternionRot);
}

glm::vec3 GE::Transform::getTranslation() const noexcept{
    return translation;
}

glm::vec3 GE::Transform::getRotation() const noexcept{
    return rotation;
}

glm::vec3 GE::Transform::getScale() const noexcept{
    return scale;
}

glm::mat4 GE::Transform::getTransformMatrix(glm::mat4 accumMatrix){
    // Check if transform matrix is updated
    if(update) {
        // Update matrix before returning
        return updateTransformMatrix(accumMatrix);
    }

    return transformMatrix;
}

bool GE::Transform::getUpdate() const noexcept{
    return update;
}

void GE::Transform::setUpdate() noexcept{
    update = true;
}