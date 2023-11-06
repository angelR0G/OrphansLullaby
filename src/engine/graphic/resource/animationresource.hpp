#pragma once

#include <vector>
#include <array>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../sceneTree/transform.hpp"
#include "resource.hpp"
#include "vertex.hpp"
#include "texture.hpp"

#include <glad/glad.h>

#include "animations/bone.hpp"
#include "animations/nodeData.hpp"

struct MeshResource;


struct AnimationResource : public Resource{
    friend struct ResourceManager;
    public:
        using BoneInfoMap = std::unordered_map<std::string, BoneInfo>;
        AnimationResource();
        ~AnimationResource();

        //Load bones data from the model for animation
        const AssimpNodeData& getRootNode();
        std::vector<GLint> getUniformBones(uint8_t);

        Bone* findBone(const std::string&);
        const BoneInfoMap getBoneIDMap();

        float getAnimationDuration();
        float getAnimationTickPerSecond();
        
    private:

        //Bones data
        AssimpNodeData parentNode{};
        float animationDuration{};
        float animationTicksPerSecond{};
        std::vector<Bone> bones{};
        BoneInfoMap m_BoneInfoMap{};

        //std::array<glm::mat4, MAX_BONES> m_FinalBoneMatrices{};

        //Private methods
        void loadResource(std::string, MeshResource*);
        void ReadMissingBones(const aiAnimation*, MeshResource*);
	    void ReadHierarchyData(AssimpNodeData&, const aiNode*);

        inline static std::vector<GLint> ufinalBonesMatrices{};
        inline static std::vector<GLint> ufinalBonesMatricesShadow{};
        inline static std::vector<GLint> ufinalBonesMatricesShadowPoint{};
        inline static bool setupuMatrix{true};
};