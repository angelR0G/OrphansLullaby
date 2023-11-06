#include "animationresource.hpp"

#include <stdexcept>

#include "../utils/assimp_helper.hpp"
#include "animations/bone.hpp"
#include "meshresource.hpp"
#include "programresource.hpp"
#include "resourceManager.hpp"

AnimationResource::AnimationResource(){}
AnimationResource::~AnimationResource(){}

void AnimationResource::loadResource(std::string path, MeshResource* model) {
    //Create new mesh
    name = path;
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_FlipWindingOrder | aiProcess_GenBoundingBoxes);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        throw std::runtime_error("ERROR::ASSIMP animation\n");
    }
    //aiMesh* assimpMesh = scene->mMeshes[scene->mRootNode->mMeshes[0]];
    aiAnimation* animation = scene->mAnimations[0];
    animationDuration = animation->mDuration;
    animationTicksPerSecond = animation->mTicksPerSecond;
    aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
    globalTransformation = globalTransformation.Inverse();
    ReadHierarchyData(parentNode, scene->mRootNode);
    ReadMissingBones(animation, model);

    if(setupuMatrix){
        ProgramResource* program = ResourceManager::Instance()->getProgram(PROGRAM_DEFAULT);
    
        for(unsigned int i{0}; i < MAX_BONES; ++i) {
            std::string sStruct = "finalBonesMatrices[" + std::to_string(i) + "]";
            ufinalBonesMatrices.push_back(glGetUniformLocation(program->getProgramId(), sStruct.c_str()));
        }

        //Send animation transform to the shadow shader
        program = ResourceManager::Instance()->getProgram(PROGRAM_SHADOWS);
    
        for(unsigned int i{0}; i < MAX_BONES; ++i) {
            std::string sStruct = "finalBonesMatrices[" + std::to_string(i) + "]";
            ufinalBonesMatricesShadow.push_back(glGetUniformLocation(program->getProgramId(), sStruct.c_str()));
        }

        program = ResourceManager::Instance()->getProgram(PROGRAM_POINT_SHADOWS);
    
        for(unsigned int i{0}; i < MAX_BONES; ++i) {
            std::string sStruct = "finalBonesMatrices[" + std::to_string(i) + "]";
            ufinalBonesMatricesShadowPoint.push_back(glGetUniformLocation(program->getProgramId(), sStruct.c_str()));
        }
        setupuMatrix = false;
    }
    
    
}

void AnimationResource::ReadMissingBones(const aiAnimation* animation, MeshResource* model) {
    using BoneInfoMap = std::unordered_map<std::string, BoneInfo>;
    int size = animation->mNumChannels;

    BoneInfoMap boneInfoMap = model->getBoneInfoMap();//getting m_BoneInfoMap from Model class
    int boneCount = model->getBoneCount(); //getting the m_BoneCounter from Model class

    //reading channels(bones engaged in an animation and their keyframes)
    for (int i{0}; i < size; ++i) {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            boneInfoMap[boneName].id = boneCount;
            boneCount++;
        }
        bones.push_back(Bone(channel->mNodeName.data,
            boneInfoMap[channel->mNodeName.data].id, channel));
    }

    m_BoneInfoMap = boneInfoMap;
}

void AnimationResource::ReadHierarchyData(AssimpNodeData& dest, const aiNode* src) {
    assert(src);

    dest.name = src->mName.data;
    dest.transformation = AssimpGLMHelpers::convertMatrixToGLMFormat(src->mTransformation);
    dest.childrenCount = src->mNumChildren;

    for (unsigned int i{0}; i < src->mNumChildren; ++i) {
        AssimpNodeData newData;
        ReadHierarchyData(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
}

const AssimpNodeData& AnimationResource::getRootNode() { 
    return parentNode; 
}

// Returns the shader location for the animation transform vector
// 0 - Normal Shader
// 1 - Shadow Shader
// 2 - Point Shadow Shader
std::vector<GLint> AnimationResource::getUniformBones(uint8_t num){
    if(num == 0)        return ufinalBonesMatrices;
    else if(num == 1)   return ufinalBonesMatricesShadow;
    else if(num == 2)   return ufinalBonesMatricesShadowPoint;

    return {};
}

Bone* AnimationResource::findBone(const std::string& name) {
    auto iter = std::find_if(bones.begin(), bones.end(),
        [&](const Bone& Bone)
        {
            return Bone.getBoneName() == name;
        }
    );
    if (iter == bones.end()) return nullptr;
    else return &(*iter);
}

using BoneInfoMap = std::unordered_map<std::string, BoneInfo>;
const BoneInfoMap AnimationResource::getBoneIDMap()  { 
    return m_BoneInfoMap;
}

float AnimationResource::getAnimationDuration(){
    return animationDuration;
}
float AnimationResource::getAnimationTickPerSecond(){
    return animationTicksPerSecond;
}