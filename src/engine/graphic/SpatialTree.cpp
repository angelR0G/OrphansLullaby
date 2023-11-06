#include "SpatialTree.hpp"

#include "sceneTree/sceneEntity.hpp"
#include "frustrumPlane.hpp"
#include "engine.hpp"

#define SPATIAL_TREE_MAX_EXTENT 4000.f
#define SPATIAL_TREE_NODE_MIN   80.f

using namespace GE;

SpatialTree::SpatialTree() {
    AABBBoundingBox spatialTreeVolume({0.f}, {SPATIAL_TREE_MAX_EXTENT});
    root = std::unique_ptr<SpatialTreeNode>(new SpatialTreeNode(nullptr, AABBBoundingBox(spatialTreeVolume)));
    root->parentNode = nullptr;
}

SpatialTreeNode::SpatialTreeNode(SpatialTreeNode* pNode, AABBBoundingBox volume) : 
    nodeVolume{volume}, parentNode{pNode} {

	// Check if node has enough size to be subdivided
	if(nodeVolume.extent.x/2 > SPATIAL_TREE_NODE_MIN) {
    	// Create children nodes
		Vec3f childrenSize{nodeVolume.extent.x/2.f};

		Vec3f center = nodeVolume.center;
		center.x -= childrenSize.x;
		center.y -= childrenSize.y;
		center.z -= childrenSize.z;
		children[0] = std::unique_ptr<SpatialTreeNode>(new SpatialTreeNode(this, AABBBoundingBox{center, childrenSize}));
		children[0]->parentIndex = 0;

		center = nodeVolume.center;
		center.x += childrenSize.x;
		center.y -= childrenSize.y;
		center.z -= childrenSize.z;
		children[1] = std::unique_ptr<SpatialTreeNode>(new SpatialTreeNode(this, AABBBoundingBox{center, childrenSize}));
		children[1]->parentIndex = 1;

		center = nodeVolume.center;
		center.x -= childrenSize.x;
		center.y += childrenSize.y;
		center.z -= childrenSize.z;
		children[2] = std::unique_ptr<SpatialTreeNode>(new SpatialTreeNode(this, AABBBoundingBox{center, childrenSize}));
		children[2]->parentIndex = 2;

		center = nodeVolume.center;
		center.x += childrenSize.x;
		center.y += childrenSize.y;
		center.z -= childrenSize.z;
		children[3] = std::unique_ptr<SpatialTreeNode>(new SpatialTreeNode(this, AABBBoundingBox{center, childrenSize}));
		children[3]->parentIndex = 3;

		center = nodeVolume.center;
		center.x -= childrenSize.x;
		center.y -= childrenSize.y;
		center.z += childrenSize.z;
		children[4] = std::unique_ptr<SpatialTreeNode>(new SpatialTreeNode(this, AABBBoundingBox{center, childrenSize}));
		children[4]->parentIndex = 4;

		center = nodeVolume.center;
		center.x += childrenSize.x;
		center.y -= childrenSize.y;
		center.z += childrenSize.z;
		children[5] = std::unique_ptr<SpatialTreeNode>(new SpatialTreeNode(this, AABBBoundingBox{center, childrenSize}));
		children[5]->parentIndex = 5;

		center = nodeVolume.center;
		center.x -= childrenSize.x;
		center.y += childrenSize.y;
		center.z += childrenSize.z;
		children[6] = std::unique_ptr<SpatialTreeNode>(new SpatialTreeNode(this, AABBBoundingBox{center, childrenSize}));
		children[6]->parentIndex = 6;

		center = nodeVolume.center;
		center.x += childrenSize.x;
		center.y += childrenSize.y;
		center.z += childrenSize.z;
		children[7] = std::unique_ptr<SpatialTreeNode>(new SpatialTreeNode(this, AABBBoundingBox{center, childrenSize}));
		children[7]->parentIndex = 7;
	}
	else {
		children[0] = nullptr;
		children[1] = nullptr;
		children[2] = nullptr;
		children[3] = nullptr;
		children[4] = nullptr;
		children[5] = nullptr;
		children[6] = nullptr;
		children[7] = nullptr;
	}
};
#include <iostream>
void SpatialTree::addEntity(const AABBBoundingBox& bbox, SceneEntity* entity) {
	// Check entity intersects entity's bounding box
	if(root->nodeVolume.intersects(bbox)) {
		root->addEntity(bbox, entity);
	}
}

void SpatialTreeNode::addEntity(const AABBBoundingBox& bbox, SceneEntity* entity) {
	size_t suitableChild{SPATIAL_TREE_DIVISION};
	// Check if can be inside one of its children
	for(size_t i{0}; i<SPATIAL_TREE_DIVISION; ++i) {
		if(children[i] == nullptr) continue;

		// Check if bounding boxes intersect
		if(children[i]->nodeVolume.intersects(bbox)) {
			if(suitableChild >= SPATIAL_TREE_DIVISION) {
				// Can be saved in this child node
				suitableChild = i;
			}
			else {
				// Intersects with multiple nodes, save here
				suitableChild = SPATIAL_TREE_DIVISION;
				break;
			}
		}
	}

	if(suitableChild == SPATIAL_TREE_DIVISION) {
		// Save entity in this node
		entities.push_back(entity);
		entity->spatialTreeNode = this;
		if(entities.size() > 100) {
			entity->spatialTreeNode = this;
		}
	}
	else {
		// Try to save it in its child
		children[suitableChild]->addEntity(bbox, entity);

		// Update children's mask
		uint8_t mask = 1 << suitableChild;
		haveChildrenEntities |= mask;
	}
}

void SpatialTree::removeEntity(SceneEntity* entity) {
	SpatialTreeNode* node {entity->spatialTreeNode};

	// Check entity is a node
	if(node == nullptr) return;
	
	for(auto it{node->entities.begin()}; it < node->entities.end() - 1; ++it) {
		if(*it == entity) {
			// Move last entity to the removed entity position
			auto lastEntity {node->entities.back()};
			*it = lastEntity;
			break;
		}
	}

	// Remove entity from node
	node->entities.pop_back();
	entity->spatialTreeNode = nullptr;

	// If all entities have been eliminated, update parent mask
	while(node->entities.empty() && node->haveChildrenEntities == 0) {
		// Get parent node and child index
		auto index 	= node->parentIndex;
		node 		= node->parentNode;

		if(node == nullptr) break;

		// Update mask
		uint8_t mask = ~(1 << index);
		node->haveChildrenEntities &= mask;
	}
}

void SpatialTree::updateEntity(SceneEntity* entity, const AABBBoundingBox& bbox) {
	SpatialTreeNode* node {entity->spatialTreeNode};

	// Check entity is in a node
	if(node == nullptr) return;

	if(!node->nodeVolume.contains(bbox)) {
		removeEntity(entity);
		addEntity(bbox, entity);
	}
}

void SpatialTreeNode::render(ShaderResource* shader, const std::vector<FrustrumPlane>& cameraFrustrum, bool useFrustum) const noexcept{
	// Check if the node is inside frustrum
	if(useFrustum && !isOnFrustrum(cameraFrustrum)) return;

	// Execute function for each entity
	for(auto e : entities) {
		e->draw(e->getTransformMatrix(), shader);
	}
	
	// Make the same with each child node
	if(haveChildrenEntities == 0) return;

	for(size_t i{0}; i < 8; ++i) {
		// Check if child has entities inside
		if((haveChildrenEntities & (1 << i)) != 0) {
			// Has entities, execute foreach
			children[i]->render(shader, cameraFrustrum);
		}
	}
}

void SpatialTree::render(ShaderResource* shader, const std::vector<FrustrumPlane>& cameraFrustrum, bool useFrustum) const noexcept {
	root->render(shader, cameraFrustrum, useFrustum);	
}

void SpatialTree::clear() noexcept {
	// Erase all tree nodes
	root = nullptr;

	// Create tree root again
	AABBBoundingBox spatialTreeVolume({0.f}, {SPATIAL_TREE_MAX_EXTENT});
    root = std::unique_ptr<SpatialTreeNode>(new SpatialTreeNode(nullptr, AABBBoundingBox(spatialTreeVolume)));
}

bool SpatialTreeNode::isOnFrustrum(const std::vector<FrustrumPlane>& camFrustum) const noexcept {
	// If node bounding box is in front of all frustum planes, node is inside frustum
	return 	(	isOnForwardPlane(camFrustum[FRUSTRUM_LEFT_FACE]) 	&&
            	isOnForwardPlane(camFrustum[FRUSTRUM_RIGHT_FACE]) 	&&
            	isOnForwardPlane(camFrustum[FRUSTRUM_TOP_FACE]) 	&&
            	isOnForwardPlane(camFrustum[FRUSTRUM_BOTTOM_FACE]) 	&&
            	isOnForwardPlane(camFrustum[FRUSTRUM_NEAR_FACE]) 	&&
            	isOnForwardPlane(camFrustum[FRUSTRUM_FAR_FACE])		);
};

bool SpatialTreeNode::isOnForwardPlane(const FrustrumPlane& plane) const noexcept{
	// Get node center and cast to glm::vec3
	glm::vec3 center {nodeVolume.center.x, nodeVolume.center.y, nodeVolume.center.z};

	// Get a projection of the bounding box onto the plane
	float r = nodeVolume.extent.x * std::abs(plane.normal.x) + nodeVolume.extent.y * std::abs(plane.normal.y) + nodeVolume.extent.z * std::abs(plane.normal.z);

	// Get distance from node center to plane
	float s = glm::dot(plane.normal, center) - plane.distance;

	return -r <= s;
}