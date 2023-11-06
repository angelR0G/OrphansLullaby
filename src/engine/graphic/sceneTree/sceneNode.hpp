#pragma once
#include <vector>
#include <memory>
#include "sceneEntity.hpp"
#include "../frustrumPlane.hpp"

struct GraphicEngine;
struct ShaderResource;

namespace GE
{
    struct SceneNode{
        friend GraphicEngine;

        SceneNode* addChild(std::unique_ptr<SceneNode>);
        void removeChild(SceneNode*);
        void setUpdateChildren();
        SceneNode* getParent();
        void run(bool, mat4, ShaderResource*, std::vector<FrustrumPlane>, bool);
        void deleteNode(bool);
        void setParent(SceneNode*);

        // Node entity
        std::unique_ptr<SceneEntity> sEntity;

        private:
            SceneNode() = default;
            std::vector<std::unique_ptr<SceneNode>> children;
            SceneNode* parent{nullptr};

    };
};//end namespace GE

