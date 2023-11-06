#pragma once

#include "../engine/physics/collisionObject.hpp"

struct PhysicsEngine;

void generateMapCollisions(std::string, PhysicsEngine&, std::vector<CollisionObject>&);