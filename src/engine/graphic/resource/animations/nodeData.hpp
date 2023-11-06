#pragma once

#include <vector>
#include <string>
#include <glm/mat4x4.hpp>

struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};