#pragma once

#include<assimp/quaternion.h>
#include<assimp/vector3.h>
#include<assimp/matrix4x4.h>
#include<glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>


struct AssimpGLMHelpers {
	static glm::mat4 convertMatrixToGLMFormat(const aiMatrix4x4& from);
	static glm::vec3 getGLMVec(const aiVector3D& vec);
	static glm::quat getGLMQuat(const aiQuaternion& pOrientation);
};