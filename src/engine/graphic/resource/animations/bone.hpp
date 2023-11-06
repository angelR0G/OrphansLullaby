#pragma once

#include <vector>
#include <string>


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

#define MAX_BONES 80

struct BoneInfo{
	//id is index in finalBoneMatrices
	int id;

	//offset matrix transforms vertex from model space to bone space
	glm::mat4 offset;

};

//Structs for position, rotation and scale for each keyframe
struct KeyPosition
{
	glm::vec3 position;
	float timeStamp;
};

struct KeyRotation
{
	glm::quat orientation;
	float timeStamp;
};

struct KeyScale
{
	glm::vec3 scale;
	float timeStamp;
};

struct Bone{

    Bone(const std::string, int, const aiNodeAnim*);

    int getPositionIndex(float animationTime);
    int getRotationIndex(float animationTime);
    int getScaleIndex(float animationTime);

    void update(float);

    [[nodiscard]] glm::mat4 getLocalTransform();
    [[nodiscard]] std::string getBoneName() const;
    [[nodiscard]] int getBoneID();

    void addPositionKeyFrame(KeyPosition);
    void addRotationKeyFrame(KeyRotation);
    void addScaleKeyFrame(KeyScale);

    private:
        std::vector<KeyPosition> m_Positions;
        std::vector<KeyRotation> m_Rotations;
        std::vector<KeyScale> m_Scales;

        int m_NumPositions;
        int m_NumRotations;
        int m_NumScalings;

        glm::mat4 m_LocalTransform;
        std::string m_Name;
        int m_ID;

        //Private methods
        float getScaleFactor(float, float, float);
        glm::mat4 interpolatePosition(float);
        glm::mat4 interpolateRotation(float);
        glm::mat4 interpolateScaling(float);
};