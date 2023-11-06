#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include <memory>
#include "transform.hpp"
#include "../frustrumPlane.hpp"
#include "../../../utils/bbox.hpp"

struct GraphicEngine;
struct ShaderResource;

namespace GE{
    struct SceneNode;
    struct SpatialTreeNode;
    struct SpatialTree;

    struct SceneEntity{
        friend GraphicEngine;
        friend SpatialTree;
        friend SpatialTreeNode;

        virtual ~SceneEntity() = default;

        virtual void draw(const glm::mat4&, ShaderResource*);
        void setTreeNode(SceneNode* sn);
        Transform* getTransform();

        void setTranslation(glm::vec3);
        void setRotation(glm::vec3);
        void setScale(glm::vec3);
        void setTransformMatrix(glm::mat4);
        [[nodiscard]] glm::vec3 getTranslation()    const noexcept;
        [[nodiscard]] glm::vec3 getWorldLocation()  const noexcept;
        [[nodiscard]] glm::vec3 getRotation()       const noexcept;
        [[nodiscard]] glm::vec3 getWorldRotation()  const noexcept;
        [[nodiscard]] glm::vec3 getScale()          const noexcept;
        [[nodiscard]] glm::mat4 getTransformMatrix(glm::mat4 = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}) noexcept;

        void setParent(SceneEntity*);

        protected:
            SceneEntity() = default;
            SceneNode* treeNode{nullptr};
            Transform transform;
            SpatialTreeNode* spatialTreeNode{nullptr};
    };
};//end namespace GE
