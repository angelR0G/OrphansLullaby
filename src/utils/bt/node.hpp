#pragma once

#include <cstdint>

struct AIComponent;
struct PhysicsEngine;
struct NavigationComponent;
struct HealthComponent;
struct AnimationComponent;
struct MovementComponent;
struct AttackStatsComponent;

// 1:basic entity | 2:basic target | 3:movement target 
// 4:ai entity | 5:physics engine | 6:nav entity 
// 7:hp entity | 8:sound entity | 9:animation entity 
// 10:dt | 11:id entity | 12:attackstat entity
// 13:enemy type
struct EntityContext {
	BasicComponent* pos;
	BasicComponent* posPlayer;
	MovementComponent* mov;
	AIComponent* ai;
	PhysicsEngine* physics;
	NavigationComponent* nav;
	HealthComponent* hp;
	SoundComponent* sc;
	AnimationComponent* animC;
	float deltaTime;
	size_t idEntity;
	AttackStatsComponent* atckstats;
	uint8_t enemyType; //0: normal | 1: explosive | 2: thrower | 3: rugby
};

enum class BTNodeStatus : uint8_t {
		success
	,	fail
	,	running
};

struct BTNode{
	struct Deleter {
		void operator()(BTNode* n) { n->~BTNode(); }
	};
	
	BTNode()		  = default;
	virtual ~BTNode() = default;

	// como no queremos que haya constructores de copia, ni operadores asignacion
	BTNode(const BTNode&)		 	 = delete;
	BTNode(BTNode&&)		 		 = delete;
	BTNode& operator=(const BTNode&) = delete;
	BTNode& operator=(BTNode&&)	 	 = delete;
	
	virtual BTNodeStatus run(EntityContext* ectx) noexcept = 0;
};