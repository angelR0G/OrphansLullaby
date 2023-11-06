#pragma once

#include <memory>
#include <vector>

#include "../../utils/bbox.hpp"

#define SPATIAL_TREE_DIVISION   8

namespace GE {
    struct SceneEntity;
    struct SpatialTree;
}

struct ShaderResource;
struct FrustrumPlane;

namespace GE {
struct SpatialTreeNode {
    friend SpatialTree;

    AABBBoundingBox                     nodeVolume;
    SpatialTreeNode*                    parentNode {nullptr};
    uint8_t                             parentIndex{};

    std::unique_ptr<SpatialTreeNode>    children[8] {};
    std::vector<GE::SceneEntity*>       entities{};

    private:
    // Masks to indicate which children has entities
    uint8_t haveChildrenEntities{false};

    SpatialTreeNode(SpatialTreeNode* pNode, AABBBoundingBox volume);
    void    addEntity(const AABBBoundingBox& bbox, GE::SceneEntity* entity);
    void    removeEntity(GE::SceneEntity*);
    void    render(ShaderResource* shader, const std::vector<FrustrumPlane>& cameraFrustrum, bool useFrustum = true) const noexcept;
    bool    isOnFrustrum(const std::vector<FrustrumPlane>& camFrustum) const noexcept;
    bool    isOnForwardPlane(const FrustrumPlane& plane) const noexcept;
};

struct SpatialTree {
    SpatialTree();

    void    addEntity(const AABBBoundingBox& bbox, GE::SceneEntity* entity);
    void    removeEntity(GE::SceneEntity* entity);
    void    updateEntity(GE::SceneEntity* entity, const AABBBoundingBox& bbox);
    void    render(ShaderResource* shader, const std::vector<FrustrumPlane>& cameraFrustrum, bool useFrustum = true) const noexcept;
    void    clear() noexcept;

    private:
        std::unique_ptr<SpatialTreeNode>    root;
};
}