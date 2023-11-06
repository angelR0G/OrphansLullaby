#pragma once

#include "../engine/types.hpp"
#include "../engine/graphic/resource/animations/bone.hpp"

#include "../engine/graphic/resource/animations/nodeData.hpp"
#include "../engine/graphic/sceneTree/sceneMesh.hpp"

struct AnimationSystem {
    AnimationSystem();
    ~AnimationSystem();
    void update(EntityMan& EM, float);

    private:
        void CalculateBoneTransform(const AssimpNodeData*, glm::mat4, AnimationComponent*);
        void activateAnimation(AnimationComponent*, GE::SceneMesh*);
};